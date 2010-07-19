<?php
// $Id:$
/**
 * @file
 * Integration with Google Fusion Tables. API class.
 */

/**
 * Google fusion client class
 *
 * This is an API wrapper for querying fusion tables
 *
 */
class Drupal_Gdata_Fusion extends Zend_Gdata_App {
  protected $_defaultPostUri = GDATA_FUSION_API_QUERY;
  protected $tableList;

  /**
   * Class constructor
   */
  public function __construct($client = NULL, $applicationId = 'DevelopmentSeed-Drupal-1.0') {
    parent::__construct($client, $applicationId);
    $this->_httpClient->setUri($this->_defaultPostUri);
  }

  /**
   * Create table
   *
   * @param $name string
   *   Table name. Use only alphanumeric characters or underscores.
   * @param $schema array()
   *   Array of multiple field => type elements.
   *   Example: array('name' => 'STRING', age => 'NUMBER');
   *   Field types are: 'NUMBER', 'STRING', 'LOCATION', 'DATETIME'
   *
   * @return int
   *   Table id if successful
   */
  public function createTable($name, $schema) {
    $fields = array();
    foreach ($schema as $field_name => $field_type) {
      $fields[] = $field_name . ': ' . $field_type;
    }
    $sql = 'CREATE TABLE ' . $name . ' (' . implode(', ', $fields) . ')';

    if ($response = $this->_query($sql)) {
      $this->tableList = NULL;
      return $response->get_value();
    }
    else {
      return FALSE;
    }
  }

  /**
   * Drop (delete) a table.
   *
   * @param $table_id
   *   Fusion Table id as returned by createTable().
   *
   * @return response code as scalar value if successful, FALSE otherwise.
   */
  public function dropTable($table_id) {
    if ($response = $this->_query('DROP TABLE '. $table_id)) {
      return $response->get_value();
    }
    else {
      return FALSE;
    }
  }

  /**
   * Insert a single row into the table
   *
   * @param $table_id int
   *   Table id
   * @param $data object or array
   *   Object or structured array containing data to store in single row
   *
   * @return int
   *   Row id of the new created. FALSE if failed
   *   In any case will be something that evaluates to TRUE the operation is successful
   */
  public function insertRow($table_id, $data) {
    $values = (array)$data;
    $sql = $this->sqlInsert($table_id, array_keys($values), $values);
    if ($response = $this->_query($sql)) {
      return (int)$response->get_value();
    }
    else {
      return FALSE;
    }
  }

  /**
   * Insert bulk data into the table
   *
   * Multiple insert statements can be grouped into a single request so this should speed up bulk operations
   *
   * @param $table_id int
   *   Table id
   * @param $fields array
   *   Array of (ordered) field names to insert into
   * @param $data array
   *   Array of arrays with (ordered) values to insert into the fields
   *
   * @return array
   *   Array of ids for newly created rows, if successful.
   */
  public function insertData($table_id, $fields, $data) {
    $statements = array();
    foreach ($data as $row) {
      $statements[] = $this->sqlInsert($table_id, $fields, $row);
    }
    $sql = implode('; ', $statements);

    if ($response = $this->_query($sql)) {
      return $response->get_column();
    }
    else {
      return FALSE;
    }
  }

  /**
   * Get tables under user account
   *
   * Note: SHOW TABLES will only display tables for which the authenticated user is listed as
   * an owner, collaborator, contributor, or viewer on the table.
   * See http://code.google.com/apis/fusiontables/docs/developers_guide.html#Exploring
   *
   * The table list is an array with table_id => table_name
   *
   * @param $refresh
   *   Refresh table list from server
   */
  public function getTables($refresh = FALSE) {
    if (!isset($this->tableList) || $refresh) {
      if ($response = $this->_query("SHOW TABLES")) {
        $this->tableList = array_combine($response->get_column(0), $response->get_column(1));
      }
      else {
        // The request failed so we just set an empty array for not to retry again
        $this->tableList = array();
      }
    }
    return $this->tableList;
  }

  /**
   * Set user table list, in case we know about the tables we are using beforehand
   *
   * This will skip querying for tables and will also allow using tables not owned by the user
   *
   * @param $tables
   *   Array with table_id => table_name
   */
  public function setTables($tables) {
    $this->tableList = $tables;
  }

  /**
   * Get Id for a table name
   *
   * @param $name
   *   Table name
   * @return int
   *   Table id or FALSE if table not found
   */
  public function getTableId($name) {
    return array_search($name, $this->getTables());
  }

  /**
   * Build simple insert statement
   */
  public static function sqlInsert($table_id, $fields, $values) {
    return 'INSERT INTO ' . $table_id . '(' . implode(', ', $fields) . ') VALUES (' . implode(', ', array_map('_gdata_fusion_escape_value', $values)) . ')';
  }

  /**
   * Run SQL query against Google Fusion tables
   *
   * The SQL has a special syntax defined here
   * http://code.google.com/apis/fusiontables/docs/developers_reference.html
   *
   * @see db_query()
   *
   * @param $sql
   *   SQL statement with placeholders for values
   * @param $args
   *   Array of arguments to be replaced in the placeholders.
   *   Just like db_query()
   *
   * @return Drupal_Gdata_Fusion_Response
   *   Response object if successful. FALSE otherwise.
   */
  public function query($query) {
    $args = func_get_args();
    array_shift($args);

    // We do table/key replacement here
    $query = $this->replace_tables($query);
    if (isset($args[0]) and is_array($args[0])) { // 'All arguments in one array' syntax
      $args = $args[0];
    }
    if ($args) {
      _gdata_fusion_query_callback($args, TRUE);
      $query = preg_replace_callback(DB_QUERY_REGEXP, '_gdata_fusion_query_callback', $query);
    }
    return $this->_query($query);
  }

  /**
   * Resolve table names to table ids.
   *
   * Replaces table names enclosed in {} with their table ids. This is similar to Drupal table prefixing.
   */
  protected function replace_tables($query) {
    $tables = array();
    foreach ($this->getTables() as $id => $name) {
      $tables['{' . $name . '}'] = $id;
    }
    return strtr($query, $tables);
  }

  /**
   * Raw query
   *
   * @param $sql
   *   SQL query
   *
   * @return Drupal_Gdata_Fusion_Response
   *   Response object if successful. FALSE otherwise.
   */
  protected function _query($sql) {
    if ($response = $this->post(array('sql' => $sql))) {
      $result = new Drupal_Gdata_Fusion_Response($response);
      $result->query = $sql;
      return $result;
    }
    else {
      // @todo Exception object
      return FALSE;
    }
  }

  /**
   * Post parameters
   */
  public function post($params) {
    $client =$this->getHttpClient();
    $client->setMethod('POST');
    foreach ($params as $name => $value) {
      $client->setParameterPost($name, $value);
    }
    try {
      return $client->request();
    }
    catch (Exception $exception) {
      // @todo There shouldn't be a visible message here. This will help with debugging though.
      drupal_set_message("Drupal_Gdata_Fusion Exception " . $exception->getMessage(), 'error');
      //print gdata_debug($exception, 'Exception');
      //print gdata_debug($this->_httpClient, 'HttpClient');
      // Still we return the response
      //$response = $exception->getResponse();
      return FALSE;
    }
  }
}

/**
 * Response to gdata query
 * @author jose
 *
 */
class Drupal_Gdata_Fusion_Response {
  // Query thrown, stored for further reference
  public $query;
  public $args;
  // Reply data
  protected $response;
  protected $csv;
  protected $data;
  protected $header;
  protected $rows;

  /**
   * Build from CSV reply
   *
   * @param $response Zend_Http_Response
   */
  public function __construct($response) {
    $this->response = $response;
    $this->csv = $response->getBody();
  }

  /**
   * Get raw CSV data
   */
  public function get_csv() {
    return $this->csv;
  }

  /**
   * Get raw array data
   */
  public function get_array() {
    if (!isset($this->data)) {
      $this->data = !empty($this->csv) ? $this->csv2array($this->csv) : array();
      $this->header = $this->data ? current($this->data) : array();
    }
    return $this->data;
  }

  /**
   * Get header (first row of data array)
   */
  public function get_header() {
    if (!isset($this->header)) {
      $this->get_array();
    }
    return $this->header;
  }

  /**
   * Get single row of data as array (skipping headers)
   */
  public function get_rows() {
    if (!isset($this->rows)) {
      $this->rows = $this->get_array();
      // Take out the header
      array_shift($this->rows);
    }
    else {
      reset($this->rows);
    }
    return $this->rows;
  }

  /**
   * Get next data row
   */
  public function get_row() {
    if (!isset($this->rows)) {
      $this->get_rows();
    }
    $value = current($this->rows);
    next($this->rows);
    return $value;
  }

  /**
   * Get row as scalar value
   */
  public function get_value() {
    if ($row = $this->get_row()) {
      return current($row);
    }
  }

  /**
   * Get first column as array of values
   *
   * @param $index
   *   Field to return
   */
  public function get_column($index = 0) {
    $values = array();
    foreach ($this->get_rows() as $row) {
      $values[] = $row[$index];
   }
   return $values;
  }

  /**
   * Format as html table
   */
  public function format_table() {
    $header = array_map('check_plain', $this->get_header());
    $rows = array_map('_gdata_fusion_check_array', $this->get_rows());
    return theme('table', $header , $rows);
  }

  /**
   * Format raw CSV as HTML output
   */
  public function format_csv() {
    return '<pre>' . check_plain($this->get_csv()) . '</pre>';
  }

  /**
   * Convert csv multiline result into array of arrays
   */
  public static function csv2array($text) {
    // Interesting, there's a function in PHP 5.3...
    // return str_getcsv($csv);
    // To work with older versions (PHP 5.2) we use this 'string-as-file trick'
    $result = array();
    $fh = fopen('php://memory', 'rw');
    fwrite($fh, $text);
    rewind($fh);
    while ($line = fgetcsv($fh)) {
      $result[] = $line;
    }
    fclose($fh);
    return $result;
  }
}

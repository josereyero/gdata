Google Fusion Tables - Drupal integration
=========================================

These modules provide a Google Fusion tables query API, that cand send SQL commands to Google 
and return results either as plain CSV text or as structured data.

For this, we got some pieces working together. The modules here are mostly glue code and the 
Fusion tables API wrapper. You'll need some steps to get it working:

0. Installation. Dependencies
-------------------------------
These modules depend on:
- Drupal's OAuth Common module, 6.x-2.0-beta2, http://drupal.org/project/oauth_common
  - Apply this patch *before* installing the module: http://drupal.org/node/846370
  - This is an obsoleted version and will be replaced soon by OAuth module, http://drupal.org/project/oauth 
- Drupal's Zend Framework module, http://drupal.org/project/zend
  - You'll need to download Zend library from http://framework.zend.com/download
  - Then go to "Administer > Site configuration > Zend Framework" and set the proper path there
- Other API modules
  - Autoload, http://drupal.org/project/autoload
  - Ctools, http://drupal.org/project/ctools
  - Input stream, http://drupal.org/project/inputstream
  
1. Configuration
----------------
- Enable Google Fusion API and all its depencencies 
- Go to "Administer > Google Data" and set up the Google authentication scope
  For Fusion Tables it should contain at least 'http://tables.googlelabs.com/api/query/'
  Let the Domain as it is ('default') because Fusion Tables doesn't work yet with Google Apps domains
- Go to "Administer > Google Data > OAuth" and set up your Application's OAuth consumer key and secret 
  You can register your application and get your Consumer secret here, 
  http://code.google.com/apis/accounts/docs/RegistrationForWebAppsAuto.html
- Optionally you can set up an RSA private key on the first tab ("Google Data"). 
  This is not really needed and just provides some extra features
  
2. Getting started
------------------
- Under your User account page ("My account") you'll see a "Google" Tab
- Go to the "Authorize" sub-tab and follow the steps: "Request token", "Authorize token"
  (This process will be made more straight forward and user friendly. It is like that ATM for debugging purposes)
- Once you get an 'Acces token' you can use either the "Query" tab to throw direct SQL queries to Fusion tables or the
  rest of the functionality provided by other modules.
  
Note: For all this you will need a 'real' Google account (like a gmail account). Google Apps accounts, like the ones hosted
under other domains won't work.

4. API Usage
------------
// This will get a client authenticated for this user (provided the User already has an access token)
   $gdata = gdata_fusion_user_get_client($user);
   
// There are some specific API methods for most common operations, to use instead of plain SQL
   $table_id = $gdata->createTable('People', array('name' => 'STRING', age => 'NUMBER'));
   $gdata->insertRow($table_id, array('name' => 'Jose', 'age' => 20);
 
// But you can also use regular Drupal queries, pretty similar to db_query(). 
// Reguar Fusion queries use 'table ids' instead of table names, but our API also has 'table prefixing' like functionality
   $gdata->query("INSERT INTO {People} (name, age) VALUES('%s', %d)", 'Jose', 21);

// For full API documentation see gdata_fusion.class.inc

// All raw Fusion Tables queries return CSV data. But our query() method will return
// a Drupal_Gdata_Fusion_Response object which has some convenience methods for parsing resulting CSV
   $result = $gdata->query("SELECT name, age FROM $table_id");
   $array = $result->get_array();
   
// At this point the resulting array should be:
   $array == array(
     array('name', 'age'), // Field names
     array('Jose', 20), // First row of data
     array('Alex', 21), // Second row of data
);

// Just remember these are regular SQL-like queries so 'raw sql' (if you don't use our db_query-like parameters)
// is vulnerable to SQL Injection attacks. And also stored data may be malicious XSS. So do don't forget check_plain()

// Have fun!!
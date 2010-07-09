<?php
// $Id:$
/**
 * @file
 * Integration with Google Fusion Tables
 * 
 */

/**
 * Test query page
 */
function gdata_fusion_query_page($account) {
  // This one redirects if no authentication
  gdata_oauth_user_check($account);

  $output = drupal_get_form('gdata_fusion_query_form', $account);

  return $output;
}

/**
 * Query form
 */
function gdata_fusion_query_form($form_state, $account) {
  // http://code.google.com/apis/gdata/authsub.html#Registered
  $response = isset($form_state['values']['response']) ? $form_state['values']['response'] : NULL;
  if (isset($response)) {
    $form['result'] = array(
      '#type' => 'fieldset',
    );
    $form['result']['raw'] = array(
      '#type' => 'item',
      '#title' => t('Raw'),
      '#value' => $response ? $response->format_csv() : t('No response'),
    );
    if ($response) {
      $form['result']['data'] = array(
        '#type' => 'item',
        '#title' => t('Response data'),
        '#value' => $response->format_table(),
      );
    }
  }
  $form['query'] = array(
    '#type' => 'textarea',
    '#title' => t('SQL Query'),
    '#default_value' => isset($form_state['values']['query']) ? $form_state['values']['query'] : '',
  );
  $form['controls']['execute'] = array('#type' => 'submit', '#value' => t('Execute'));

  return $form;
}

/**
 * Query form submit
 */
function gdata_fusion_query_form_submit($form, &$form_state) {
  $op = !empty($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $account = $form_state['values']['account'];

  switch ($op) {
    case t('Execute'):
      $query = $form_state['values']['query'];
      $gdata = gdata_fusion_user_get_client($account);
      $form_state['values']['response'] = $gdata->query($query);
      $form_state['rebuild'] = TRUE;  
      break;
  }
}

/**
 * Format result as HTML
 */
function gdata_fusion_format_result($response) {
  if ($response) {
    return theme('table', $response->get_header(), $response->get_rows());
  }
  else {
    return t('No result');
  }
}
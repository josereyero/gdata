<?php

/**
 * Overview page
 */
function gdata_page_overview() {
  global $user;
  return gdata_pages_user_account($account);
}

/**
 * User account overview
 */
function gdata_pages_user_account($account) {
  // This one redirects if no authentication
  gdata_auth_user_check($account);

  $output = '';

  $output .= drupal_get_form('gdata_user_account_form', $account);

  return $output;
}

/**
 * User account form
 */
function gdata_user_account_form($form_state, $account) {
  $form['account'] = array('#type' => 'value', '#value' => $account);
  $form['status'] = array(
    '#type' => 'item',
    '#title' => t('Status'),
    '#value' => t('You are authenticated with Google.'),
  );
  // If we have a user token we can check the token
  if ($token = gdata_auth('get_user_token', $account)) {
    $form['token'] = array('#type' => 'value', '#value' => $token);
    $form['options']['check'] = array('#type' => 'submit', '#value' => t('Check token'), '#weight' => 10);
  }
  return $form;
}

/**
 * User account form submission
 */
function gdata_user_account_form_submit($form, &$form_state) {
  $op = !empty($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $account = $form_state['values']['account'];
  
  switch ($op) {
    case t('Check token');
      $token = $form_state['values']['token'];
      $result = gdata_get_token_info($token);
      drupal_set_message($result);
      break;
  }
}
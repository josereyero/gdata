<?php
/**
 * Google Apps pages
 */

/**
 * User authentication
 */
function gdata_oauth_user_account_form($form_state, $account, $consumer = NULL) {
  $consumer = $consumer ? $consumer : gdata_oauth_get_consumer();
  $form['account'] = array('#type' => 'value', '#value' => $account);
  $form['consumer'] = array('#type' => 'value', '#value' => $consumer);
  // These are GoogleOauthToken objects
  $request_token = oauth_google_get_user_token($account, $consumer, 'request');
  $access_token = oauth_google_get_user_token($account, $consumer, 'access');
  $form['request_token'] = array('#type' => 'value', '#value' => $request_token);
  $form['access_token'] = array('#type' => 'value', '#value' => $access_token);

  $form['status'] = array(
    '#type' => 'item',
    '#title' => t('Status'),
  );
  $form['options'] = array('#type' => 'fieldset');
  if (!$access_token && !$request_token) {
    $form['options']['request'] = array('#type' => 'submit', '#value' => t('Request token'));
  }
  else {
    $tokens = array();
    if ($request_token) {
      $tokens[] = array(t('Request token'), $request_token->authorized ? t('Authorized') : t('Needs authorization'), $request_token->key, format_date($request_token->created));
    }
    if ($access_token) {
      $tokens[] = array(t('Access token'), t('OK'), $access_token->key, format_date($access_token->created));
      $status = t('You already have an access token.');
      // We can check the token if we have a private key
      $can_check = (bool)gdata_get_private_key();
      $form['options']['check'] = array('#type' => 'submit', '#value' => t('Check token'), '#weight' => 10, '#disabled' => !$can_check);
    }
    else {
      if ($request_token->authorized) {
        $status = t('You have an already authorized token.');
      }
      else {
        $status = t('You have a request token not authorized yet.');
        $form['options']['authorize'] = array('#type' => 'submit', '#value' => t('Authorize token'));
      }
    }
    $form['options']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
    $header = array(t('Type'), t('Status'), t('Token'), t('Date'));
    $form['tokens'] = array(
      '#type' => 'item',
      '#value' => theme('table', $header, $tokens),
    );
  }
  $form['status']['#value'] = isset($status) ? $status : '';
  return $form;
}

/**
 * User authentication, form submission
 */
function gdata_oauth_user_account_form_submit($form, $form_state) {
  $op = !empty($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $account = $form_state['values']['account'];
  $consumer = $form_state['values']['consumer'];
  $access_token = $form_state['values']['access_token'];
  $request_token = $form_state['values']['request_token'];

  switch ($op) {
    case t('Reset'):
      if ($access_token) {
        DrupalOauthToken::deleteToken($access_token->key, FALSE);
      }
      if ($request_token) {
        DrupalOauthToken::deleteToken($request_token->key, FALSE);
      }
      break;
    case t('Request token');
      $client = new GoogleOAuthClient($consumer);
      $client->scope = gdata_oauth_get_scope();
      $token = $client->getRequestToken();
      if ($token) {
        $token->uid = $account->uid;
        $token->write();
        drupal_set_message(t('You got a new request token'));
      }
      else {
        drupal_set_message(t('Your token request failed'), 'error');
      }
      break;
    case t('Authorize token');
      $client = new GoogleOAuthClient($consumer, $request_token);
      $url = $client->getAuthorizationUrl(variable_get('oauth_google_default_domain', 'default'));
      drupal_goto($url);
      break;

    case t('Check token');
      $token = gdata_oauth_get_user_token($account);
      $result = gdata_oauth_get_token_info($token);
      drupal_set_message(check_plain($result));
      break;
  }
}

<?php
/**
 * Google Apps site wide settings
 */
function gdata_oauth_admin_settings_form() {
  $form['consumer'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth consumer'),
    '#description' => t('You can get these values <a href="@google-register">registering your application with Google</a>.', array('@google-register' => 'http://code.google.com/apis/accounts/docs/RegistrationForWebAppsAuto.html')),
  );
  $form['consumer']['gdata_oauth_consumer_key'] = array(
    '#type' => 'textfield',
    '#maxlength' => 32,
    '#title' => t('Key'),
    '#default_value' => variable_get('gdata_oauth_consumer_key', ''),
    '#required' => TRUE,
  );
  $form['consumer']['gdata_oauth_consumer_secret'] = array(
    '#type' => 'textfield',
    '#maxlength' => 32,
    '#title' => t('Secret'),
    '#default_value' => variable_get('gdata_oauth_consumer_secret', ''),
    '#required' => TRUE,
  );
  return system_settings_form($form);
}


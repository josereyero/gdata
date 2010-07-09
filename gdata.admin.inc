<?php
/**
 * Google Apps site wide settings
 */
function gdata_admin_settings_form() {
  $form['gdata_default_scope'] = array(
    '#type' => 'textarea',
    '#title' => t('Scope'),
    '#default_value' => variable_get('gdata_default_scope', GDATA_DEFAULT_SCOPE),
    '#required' => TRUE,
  );
  $form['gdata_default_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain'),
    '#default_value' => variable_get('gdata_default_domain', 'default'),
    '#description' => t('Google Apps domain. Let default value to use regular Google accounts.'),
  );
  $form['keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('Private keys'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('You need an RSA private key only to use AuthSub requests.'),
  );
  // http://code.google.com/apis/gdata/authsub.html#Registered
  $form['keys']['gdata_default_private_key'] = array(
    '#type' => 'textarea',
    '#title' => t('Private RSA key'),
    '#default_value' => variable_get('gdata_default_private_key', ''),
  );
  return system_settings_form($form);
}

/**
 * Google apps scopes
#
Fusion Tables
http://tables.googlelabs.com/api/query
#
Analytics
https://www.google.com/analytics/feeds/
#
Google Base
http://www.google.com/base/feeds/
#
Book Search
http://www.google.com/books/feeds/
#
Blogger
http://www.blogger.com/feeds/
#
Calendar
http://www.google.com/calendar/feeds/
#
Contacts
http://www.google.com/m8/feeds/
#
Documents List
http://docs.google.com/feeds/
#
Finance
http://finance.google.com/finance/feeds/
#
GMail
https://mail.google.com/mail/feed/atom
#
Health
https://www.google.com/health/feeds/
#
H9
https://www.google.com/h9/feeds/
#
Maps
http://maps.google.com/maps/feeds/
#
OpenSocial
http://www-opensocial.googleusercontent.com/api/people/
#
orkut
http://www.orkut.com/social/rest
#
Picasa Web
http://picasaweb.google.com/data/
#
Sidewiki
http://www.google.com/sidewiki/feeds/
#
Sites
http://sites.google.com/feeds/
#
Spreadsheets
http://spreadsheets.google.com/feeds/
#
Webmaster Tools
http://www.google.com/webmasters/tools/feeds/
#
YouTube
http://gdata.youtube.com
 */
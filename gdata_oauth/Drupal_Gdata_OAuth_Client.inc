<?php
/**
 * @file
 *   PHP Class: Gdata Drupal specific Http Client
 * @author jose
 *
 */
class Drupal_Gdata_OAuth_Client extends Zend_Oauth_Client {
  // User account the client is linked to
  protected $user;
  // OAuth Drupal consumer
  protected $consumer;
  // Version
  const VERSION = '1.0';
  /**
   * Default constructor
   * 
   * @param DrupalOauthConsumer $consumer
   * 
   * @param Zend_Oauth_Token_Access $token
   */
  function __construct($consumer = NULL, $token = NULL) {
    $this->consumer = $consumer ? $consumer : gdata_oauth_get_consumer();

    $oauthOptions = array(
      'requestScheme' => Zend_Oauth::REQUEST_SCHEME_HEADER,
      'version' => '1.0',
      'consumerKey' => $this->consumer->key,
      'consumerSecret' => $this->consumer->secret,
      'signatureMethod' => 'HMAC-SHA1',
      //'callbackUrl' => $RETURN_TO,
      'requestTokenUrl' => 'https://www.google.com/accounts/OAuthGetRequestToken',
      'userAuthorizationUrl' => 'https://www.google.com/accounts/OAuthAuthorizeToken',
      'accessTokenUrl' => 'https://www.google.com/accounts/OAuthGetAccessToken'
    
    );   
    parent::__construct($oauthOptions);
   
    // Set default values
    $useragent = 'Drupal_Gdata_Fusion/' . self::VERSION;
    $this->setConfig(array(
       'strictredirects' => true,
       'useragent' => $useragent
    ));

    if ($token) {
      $this->setToken($token);
    }
  }
  
  /**
   * Set authentication credentials for an user account
   */
  function setDrupalUser($account) {
    $this->user = $account;
    
    if ($token = gdata_oauth_get_user_token($account, $this->consumer)) {
      $this->setToken($token);
    }

  }
}
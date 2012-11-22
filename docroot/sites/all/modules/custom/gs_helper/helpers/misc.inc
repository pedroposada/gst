<?php
/**
 * @file
 * Contains miscellaneous helper code that's not Drupal-specific.
 */

/**
 * Validates the specified string as an MD5 hash. Returns boolean.
 * @return 
 * @param object $hash
 */
function valid_md5($hash) {
  return preg_match("#^[0-9a-f]{32}$#i", $hash);
}

/**
 * Prepends 'http' to the URL, if missing.
 */
function ensure_http(&$url) {
  if (!preg_match('/^https{0,1}:\/\//', $url)) {
    $url = 'http://' . $url;
  }
}


/**
 * remove "information about text format" box
 * from inout forms
 */
function remove_tips(&$form) {
  unset($form['und'][0]['format']['guidelines']);
  unset($form['und'][0]['format']['help']);
  return $form;
}
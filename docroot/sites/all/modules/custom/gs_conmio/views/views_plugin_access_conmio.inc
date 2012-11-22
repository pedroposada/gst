<?php

/**
 * Access plugin that provides access to Conmio hosts.
 */
class views_plugin_access_conmio extends views_plugin_access {
  function summary_title() {
    return t('Conmio');
  }
  
  function access($account) {
    return is_conmio_host();
  }
  
  function get_access_callback() {
    return array('is_conmio_host', array());
  }
}
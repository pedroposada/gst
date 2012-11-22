<?php

/**
 * Implements hook_admin_settings() for configuring the module().
 */
function gs_admanager_admin_attributes_form($form, &$form_state) {
  $form = array();
  return system_settings_form($form);
}

/**
 * Implements hook_admin_settings_form_validate().
 */
function gs_admanager_admin_attributes_form_validate($form, &$form_state) {
}

/**
 * Implements hook_admin_settings_form_submit().
 */
function gs_admanager_admin_attributes_form_submit($form, &$form_state) {
  //$result = db_query("SELECT bid, delta FROM {block} WHERE module = :module", array(':module' => 'gs_admanager'));
}
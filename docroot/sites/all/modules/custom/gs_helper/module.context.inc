<?php
/**
 * @file
 * Contains code specific to the Context module.
 */

/**
 * Implementation of hook_ctools_plugin_type(); 
 */
function gs_helper_ctools_plugin_type() {
  return array(
    'plugins' => array(
      'cache' => TRUE,
      'use hooks' => TRUE,
      'classes' => array('handler'),
    ),
  );
}

/**
 * Implementation of hook_context_registry();
 */
function gs_helper_context_registry() {
  $registry = array();
  
  $registry['conditions'] = array(
    'gs_helper_section' => array(
      'title' => t('GS - Section'),
      'description' => t('Activates context when a section of the site is being viewed.'),
      'plugin' => 'gs_helper_context_condition_section',
    ),
  );
  
  return $registry;
}

/**
 * Implementation of hook_context_plugins();
 */
function gs_helper_context_plugins() {
  $plugins = array();
  
  $plugins['gs_helper_context_condition_section'] = array(
    'handler' => array(
      'path' => GS_HELPER_PATH . '/includes/context',
      'file' => 'gs_helper_context_condition_section.inc',
      'class' => 'gs_helper_context_condition_section',
      'parent' => 'context_condition',
    ),
  );
  
  return $plugins;
}

/**
 * Implementation of hook_context_page_condition();
 */
function gs_helper_context_page_condition() {
  if ($plugin = context_get_plugin('condition', 'gs_helper_section')) {
    $plugin->execute();
  }
}
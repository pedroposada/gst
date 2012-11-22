<?php

/**
 * Implementation of WYSIWYG's hook_INCLUDE_plugin().
 */
function gs_helper_gsmedia_plugin() {
  // Include the required browser JS.
  // @todo: wyswiyg should allow libraries and multiple js files
  // to be defined by this hook.
  // @see http://drupal.org/node/1039076
  // media_include_browser_js();

  // Plugin definition
  $plugins['gsmedia'] = array(
    'title' => t('GS Media Browser'),
    // 'vendor url' => 'http://drupal.org/project/media',
    'icon path' => drupal_get_path('module', 'gs_helper') . '/wysiwyg_plugins',
    'icon file' => 'photo-option-add.png',
    'icon title' => t('Add media (GS Media Browser)'),
    'js path' => drupal_get_path('module', 'gs_helper') . '/wysiwyg_plugins',
    'js file' => 'gsmedia.js',
    'css file' => NULL,
    'css path' => NULL,
    'settings' => array(
      'global' => array(
        'types' => media_variable_get('wysiwyg_allowed_types'),
        'id' => 'gsmedia_wysiwyg',
      ),
    ),
  );

  return $plugins;
}
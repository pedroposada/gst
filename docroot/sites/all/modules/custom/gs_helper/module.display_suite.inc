<?php
/**
 * @file
 * Contains code specific to the Display Suite module.
 */


/**
 * Implementation of hook_ctools_plugin_api();
 */
function gs_helper_ctools_plugin_api($module, $api) {
  if (($module == 'ds' && $api == 'ds') || ($module == 'gs_helper' && $api == 'gs_helper')) {
    return array('version' => 1);
  }
}

/**
 * Implementation of hook_ds_fields_info();
 */
function gs_helper_ds_fields_info($entity_type) {
  $data = array();

  // fields for common entity types
  $types = array('node', 'comment');
  if (in_array($entity_type, $types)) {
    $data[$entity_type]['gs_helper_author_date'] = array(
      'title' => t('Author & Submitted Date'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_author_date' => t('Author & Date'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_comment_rate_widget'] = array(
      'title' => t('GS Rate Widget'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_comment_rate_widget' => t('GS Rate Widget'), 
        ),
      )
    );
  }
  
  // field for comments
  if ($entity_type == 'comment') {
    $data[$entity_type]['gs_helper_comment_post_date'] = array(
      'title' => t('GS Post Date'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_comment_post_date' => t('GS Post Date'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_comment_author'] = array(
      'title' => t('GS Author'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_comment_author' => t('GS Author'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_comment_rate_widget'] = array(
      'title' => t('GS Rate Widget'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_comment_rate_widget' => t('GS Rate Widget'), 
        ),
      )
    );
  }
  
  // fields only for nodes
  if ($entity_type == 'node') {
    $data[$entity_type]['gs_helper_featured_media'] = array(
      'title' => t('GS Featured Media'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', 'blog_post|*', 'contest|*'),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_featured_media' => t('GS Featured Media'), 
        ),
        'settings' => array(
           'width'=>array('title'=>'Width of video'),
           'wide_image_style_name'=>array('title'=>'Image Style Name'),
         ),
        'default' => array('wide_image_style_name'=>'article_main','width'=>600),
      )
    );
    $data[$entity_type]['gs_helper_node_thumbnail'] = array(
      'title' => t('GS Node Thumbnail'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', 'blog_post|*'),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_node_thumbnail' => t('GS Node Thumbnail'), 
        ),
      )
    );
    // dpm($entity_type);
    $data[$entity_type]['gs_helper_node_thumbnail_universal'] = array(
      'title' => t('GS Node Universal Thumbnail'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', 'blog_post|*', 'gallery|*', 'contest|*'),
      'properties' => array(
        'formatters' => array(
          'gs_helper_node_thumbnail_universal' => t('GS Node Universal Thumbnail'), 
        ),
        'settings' => array(
           'image_style'=>array('title'=>'Image Style Name'),
           'link_content'=>array('title'=>'Link to content'),
           'display_cropped'=>array('title'=>'Display cropped thumbnail if available'),
           'file_type'=>array('title'=>"Filter thumbnail by file type ('video','image','any')"),
         ),
        'default' => array('image_style'=>'thumbnail','display_cropped'=>1,'link_content'=>1,'file_type'=>'any'),
      )
    );
    $data[$entity_type]['gs_helper_photos_tab'] = array(
      'title' => t('GS Photos Tab'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*','gallery|*'),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_photos_tab' => t('GS Photos Tab'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_body_first_paragraph'] = array(
      'title' => t('Body - First Paragraph'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', 'contest|*'),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_body_first_paragraph' => t('First paragraph'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_sidebar_node'] = array(
      'title' => t('Node Sidebar'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array(),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_sidebar_node' => t('Node sidebar'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_section_topic'] = array(
      'title' => t('Section or Topic (lowest child)'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_section_topic' => t('Link'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_embedded_videos'] = array(
      'title' => t('GS Embedded Videos'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_embedded_videos' => t('GS Embeded Videos'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_downloads_tab'] = array(
      'title' => t('GS Downloads Tab'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('article|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_downloads_tab' => t('GS Downloads Tab'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_blog_tags'] = array(
      'title' => t('GS Blog Tags'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('blog_post|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_blog_tags' => t('GS Blog Tags'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_prevnext'] = array(
      'title' => t('GS Prev Next Links'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('blog_post|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_prevnext' => t('GS Prev Next Links'), 
        ),
      )
    );
    $data[$entity_type]['gs_helper_media_gallery'] = array(
      'title' => t('GS Media Gallery'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('blog_post|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_media_gallery' => t('GS Media Gallery'), 
        ),
      )
    );
    /*
    // Commented out because template.php already handles the output and it's styled.
    // This can be reimplemented if flexibility is desired.
    $data[$entity_type]['gs_helper_sidebar_blog'] = array(
      'title' => t('GS Legacy Sidebar'),
      'field_type' => DS_FIELD_TYPE_THEME,
      'ui_limit' => array('blog_post|*', ),
      'properties' => array(
        'default' => array('wrapper' => 'div', 'link' => 0),
        'formatters' => array(
          'gs_helper_sidebar_blog' => t('Default'), 
        ),
      )
    );
    */
  }

  return $data;
}

/**
 * Implements hook_ds_field_settings_form()
 * 
 * Return a settings form for a Display Suite field.
 *
 * As soon as you have hook_ds_fields and one of the fields
 * has a settings key, Display Suite will call this hook for field form.
 *
 * @param $field
 *   The configuration of the field.
 *
 * @return $form
 *   A form definition.
 */
function gs_helper_ds_field_settings_form($field) {
  // universal thumbnail
  if ($field['name'] == 'gs_helper_node_thumbnail_universal') {
    $form["image_style"] = array(
      '#title' => $field['properties']['settings']['image_style']['title'],
      '#type' => 'select',
      '#options' => drupal_map_assoc(array_keys(image_styles())),
      '#default_value'=>isset($field['formatter_settings']['image_style']) ? 
        $field['formatter_settings']['image_style'] : $field['properties']['default']['image_style'],
    );
    $form["link_content"] = array(
      '#title' => $field['properties']['settings']['link_content']['title'],
      '#type' => 'select',
      '#options' => array(1=>'Yes',0=>'No'),
      '#default_value'=>isset($field['formatter_settings']['link_content']) ? 
        $field['formatter_settings']['link_content'] : $field['properties']['default']['link_content'],
    );
    $form["display_cropped"] = array(
      '#title' => $field['properties']['settings']['display_cropped']['title'],
      '#type' => 'select',
      '#options' => array(1=>'Yes',0=>'No'),
      '#default_value'=>isset($field['formatter_settings']['display_cropped']) ? 
        $field['formatter_settings']['display_cropped'] : $field['properties']['default']['display_cropped'],
    );
    $form["file_type"] = array(
      '#title' => $field['properties']['settings']['file_type']['title'],
      '#type' => 'select',
      '#options' => drupal_map_assoc(array('any','video')),
      '#default_value'=>isset($field['formatter_settings']['file_type']) ? 
        $field['formatter_settings']['file_type'] : $field['properties']['default']['file_type'],
    );
  }
  // featured media
  if ($field['name'] == 'gs_helper_featured_media') {
    $form["width"] = array(
      '#title' => $field['properties']['settings']['width']['title'],
      '#type' => 'select',
      '#options' => drupal_map_assoc(array(600,420,300)),
      '#default_value'=>isset($field['formatter_settings']['width']) ? 
        $field['formatter_settings']['width'] : $field['properties']['default']['width'],
    );
    $form["wide_image_style_name"] = array(
      '#title' => $field['properties']['settings']['wide_image_style_name']['title'],
      '#type' => 'select',
      '#options' => drupal_map_assoc(array_keys(image_styles())),
      '#default_value'=>isset($field['formatter_settings']['wide_image_style_name']) ? 
        $field['formatter_settings']['wide_image_style_name'] : $field['properties']['default']['wide_image_style_name'],
    );
  }
  
  return $form;
}
/**
 * Implements hook_ds_field_format_summary()
 * 
 * Return configuration summary for the field format.
 *
 * As soon as you have hook_ds_fields and one of the fields
 * has a settings key, Display Suite will call this hook for the summary.
 *
 * @param $field
 *   The configuration of the field.
 *
 * @return $summary
 *   The summary to show on the Field UI.
 */
function gs_helper_ds_field_format_summary($field) {
  $output = '';
  // $settings = isset($field['formatter_settings']) ? $field['formatter_settings'] : $field['properties']['default'];
  foreach ((array)$field['properties']['settings'] as $key => $value) {
    if (isset($field['formatter_settings']) && isset($field['formatter_settings'][$key])) {
      $output .= "<p>{$value['title']}: {$field['formatter_settings'][$key]}</p>";
    } else {
      $output .= "<p>{$value['title']}: {$field['properties']['default'][$key]}</p>";
    }
  }
  
  return $output;
}
/**
 * Implements hook_ds_layout_settings_alter()
 * 
 * Modify the layout settings just before they get saved.
 *
 * @param $record
 *   The record just before it gets saved into the database.
 * @param $form_state
 *   The form_state values.
 */
function gs_helper_ds_layout_settings_alter($record, $form_state) {
  // $record->settings['hide_page_title'] = TRUE;
  // dpm($record);
}
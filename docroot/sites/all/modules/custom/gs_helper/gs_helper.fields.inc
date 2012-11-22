<?php
/**
 * @file
 * Contains code specific to internal fields functionality.
 */

/**
 * Implementation of hook_field_formatter_info();
 */
function gs_helper_field_formatter_info() {
  $formatters = array(
    'gs_helper_field_media_photo' => array(
      'label' => t('GS - Photo Item'),
      'field types' => array('file'),
    ),
    'gs_helper_field_video' => array(
      'label' => t('GS - Video Item'),
      'field types' => array('file'),
    ),
    'gs_helper_field_image_preset' => array(
      'label' => t('GS - Image Preset'),
      // TODO: This line causes saving errors in /admin/structure/file-types
      'field types' => array('file'),
      'settings' => array(
        'image_style' => '',
        'link' => '',
        'filename' => '',
        'cutline' => '',
        'markup' => '',
        'media_type' => array(),
      ),
    ),
  );
  return $formatters;
}

/**
 * Implementation of hook_field_formatter_settings_form();
 */
function gs_helper_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $element = array();

  if ($display['type'] == 'gs_helper_field_image_preset') {
    $options = array();
    
    foreach (image_styles() as $style) {
      $options[$style['name']] = $style['name'];
    }
    
    $element['image_style'] = array(
      '#title' => t('Image style'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $settings['image_style'],
      '#required' => TRUE,
    );
    
    $element['link'] = array(
      '#title' => t('Link'),
      '#type' => 'radios',
      '#description' => t('Where should this image link?'),
      '#options' => _gs_helper_fields_image_style_link_options(),
      '#required' => TRUE,
      '#default_value' => $settings['link'],
    );
    
    $element['filename'] = array(
      '#title' => t('Display filename'),
      '#type' => 'checkbox',
      '#default_value' => isset($settings['filename']) ? $settings['filename'] : '',
    );
    
    $element['cutline'] = array(
      '#title' => t('Display cutline & credit'),
      '#type' => 'checkbox',
      '#default_value' => isset($settings['cutline']) ? $settings['cutline'] : '',
    );
    
    $element['markup'] = array(
      '#title' => t('Markup'),
      '#description' => t('Anything entered here will be displayed immediately after the image.'),
      '#type' => 'textfield',
      '#default_value' => $settings['markup'],
    );
    
    $media_type_info = field_info_field('field_media_type');
    $media_type_defaults = array();
    
    foreach ($settings['media_type'] as $key => $value) {
      if ($value) {
        $media_type_defaults[] = $key;
      }
    }
    
    $element['media_type'] = array(
      '#title' => t('Media type'),
      '#type' => 'checkboxes',
      '#options' => $media_type_info['settings']['allowed_values'],
      '#default_value' => $media_type_defaults,
      '#description' => t('What media types should be displayed? If none are checked, all will be displayed.'),
    );
  }

  return $element;
}

/**
 * Implementation of hook_field_formatter_settings_summary();
 */
function gs_helper_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = '';

  if ($display['type'] == 'gs_helper_field_image_preset') {
    $options = _gs_helper_fields_image_style_link_options();
    
    if (isset($settings['image_style'])) {
      $summary .= t('Image style: @style', array('@style' => $settings['image_style']));
      $summary .= '<br />';
    }
    
    if (isset($settings['link'])) {
      $summary .= t('Link: @link', array('@link' => $settings['link']));
      $summary .= '<br />';
    }
    
    if (isset($settings['filename'])) {
      $summary .= t('Filename: @filename', array('@filename' => $settings['filename'] ? t('Yes') : t('No')));
      $summary .= '<br />';
    }

    if (isset($settings['cutline'])) {
      $summary .= t('Cutline & credit: @cutline', array('@cutline' => $settings['cutline'] ? t('Yes') : t('No')));
      $summary .= '<br />';
    }
    
    if (isset($settings['markup'])) {
      $summary .= t('Markup: @markup', array('@markup' => check_plain($settings['markup'])));
      $summary .= '<br />';
    }
    
    if (isset($settings['media_type'])) {
      $media_type_info = field_info_field('field_media_type');
      
      $types = array();
      foreach ($settings['media_type'] as $key => $value) {
        if ($value) {
          $types[] = $media_type_info['settings']['allowed_values'][$key];
        }
      }
      
      $summary .= t('Media types: @types', array('@types' => implode(', ', $types)));
    }
  }
  
  if (empty($summary)) {
    $summary = t('No options set.');
  }

  return $summary;
}


/**
 * Implementation of hook_field_formatter_view();
 */
function gs_helper_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    case 'gs_helper_field_media_photo':
      
      foreach ($items as $delta => $file) {
        // we exclude the 'thumbnail' images from the slideshow
        $media_types = _gs_helper_extract_select_values($file['field_media_type']);
        if (!in_array('thumbnail', $media_types)) {
          $element[$delta] = array(
            '#theme' => 'gs_helper_field_media_photo',
            '#file' => $file,
          );
        }
      }
      
      break;
      
    case 'gs_helper_field_video':
      
      foreach ($items as $delta => $file) {
        // list from the supported codecs in videojs module
        $codecs = array(
          'video/mp4' => 'video/mp4',
          'video/webm' => 'video/webm',
          'application/octet-stream' => 'video/webm',
          'video/ogg' => 'video/ogg',
          'application/ogg' => 'video/ogg',
          'video/quicktime' => 'video/mp4',
          'video/flv' => 'video/flv',
          'video/x-flv' => 'video/flv',
        );
        
        if (array_key_exists($file['filemime'], $codecs) && isset($file['field_thumbnail']['und'][0]['fid'])) {
          $element[$delta] = array(
            '#theme' => 'gs_helper_field_video',
            '#video_file' => $file,
            '#image_file' => $file['field_thumbnail']['und'][0],
          );
        }
      }
      
      break;
      
    case 'gs_helper_field_image_preset':
      
      $filters = array();
      if (isset($display['settings']['media_type'])) {
        foreach ($display['settings']['media_type'] as $key => $value) {
          if ($value) {
            $filters[] = $key;
          }
        }
      }
      
      foreach ($items as $delta => $file) {
        $file_entity = $file;
        $image_entity = $file;

        if ($file['type'] !== 'image') {
          $image_entity = NULL;
          
          if (isset($file['field_thumbnail']['und'][0])) {
            $image_entity = $file['field_thumbnail']['und'][0];
          }
          else {
            $image_entity = array(
              'uri' => base_path() . drupal_get_path('theme', 'gs') . '/img/icons/photo-placeholder.jpg',
              'filename' => $file['filename'],
              'not_stream' => TRUE,
            );
          }
        }
        
        if (!empty($filters)) {
          $types = array();
          
          if (isset($image_entity['field_media_type']['und'][0]['value'])) {
            foreach ($image_entity['field_media_type']['und'] as $option) {
              $types[] = $option['value'];
            }
          }

          if (empty($types) || !array_intersect($types, $filters)) {
            continue;
          }
        }
        
        $link = '';
        
        switch ($display['settings']['link']) {
          case 'node':
            $link = 'node/' . $entity->nid;
            
            break;
            
          case 'image':
            $link = file_create_url($image_entity['uri']);
            
            break;
            
          case 'file':
            $link = file_create_url($file_entity['uri']);
            
            break;
        }
        
        $element[$delta] = array(
          '#theme' => $display['type'],
          '#file' => $image_entity,
          '#entity' => $file_entity,
          '#image_style' => $display['settings']['image_style'],
          '#link' => $link,
          '#filename' => (isset($display['settings']['filename']) && $display['settings']['filename']) ? $file_entity['filename'] : '',
          '#cutline' => (isset($display['settings']['cutline']) && $display['settings']['cutline']) ? 1 : 0,
          '#markup' => $display['settings']['markup'],
        );
      }
      
      break;
  }
  
  return $element;
}

function _gs_helper_fields_image_style_link_options() {
  $data = array(
    'none' => t('None'),
    'node' => t('Node'),
    'image' => t('Image'),
    'file' => t('File'),
  );
  
  return $data; 
}



/**
 * Implements hook_field_widget_form_alter()
 */
function gs_helper_field_widget_form_alter(&$element, &$form_state, $context) {
  
  if (isset($element['#type']) && $element['#type'] == 'media') {
    $element['#pre_render'][] = 'gs_helper_field_media_pre_render';
  }
  // if ($context['field']['type'] == 'field_mediacollection') {
  // }
  // dpm($element);
  
}
function gs_helper_field_media_pre_render($element) {
  // dpm($element);
  
  // if (isset($element['preview']['col2'])) {
//     
  // }
  
  return $element;
}

/**
 * Implements hook_element_info_alter().
 *
 * Sets the text format processor to a custom callback function.
 */
function gs_helper_element_info_alter(&$type) {
  if (isset($type['text_format']['#process'])) {
    foreach ($type['text_format']['#process'] as &$callback) {
      if ($callback === 'filter_process_format') {
        $callback = 'gs_helper_filter_process_format';
      }
    }
  }
}
/**
* Callback for gs_helper_element_info_alter().
*/
function gs_helper_filter_process_format($element) {
  $element = filter_process_format($element);
  $node_type = '';
  $field_name = '';
  
  // Array of node types and their fields to specify a default text format for.
  // TODO: Find a more elegant way of maintaining this.
  $text_format_defaults = array(
    // Article node type
    'article' => array(
      'body' => 'full_html_with_paging',
      'field_legacy_sidebar' => 'limited_html',
      'field_footer' => 'limited_html',
    ),
    // Blog node type
    'blog_post' => array(
      'body' => 'full_html',
      'field_legacy_sidebar' => 'limited_html',
      'field_footer' => 'limited_html',
    ),
    // Page node type
    'page' => array (
      // 'body' => 'full_html_with_paging'
      'body' => 'full_html'
    ),
    // Contest node type
    'contest' => array (
      'body' => 'full_html',
      'field_prizing' => 'full_html',
      'field_skill_testing_question' => 'full_html',
    ),
    // Comments
    'comment' => array (
      'comment_body' => 'plain_text',
    ),
    // Webform admin backend
    'webform' => array (
      'edit-confirmation' => 'full_html',
      'body' => 'full_html',
      'edit-value' => 'full_html',
    ),
    // Product display
    'product_display' => array (
      'body' => 'full_html',
    ),
    // Gallery
    'gallery' => array (
      'body' => 'full_html',
    ),
  );
  
  if (isset($element['#bundle']) && isset($element['#field_name']) && isset($element['#entity_type'])) {
    if ($element['#entity_type'] == 'comment') {
      // To avoid declaring comment_body for each bundle, reassign node_type based on entity_type
      $node_type = $element['#entity_type'];
      
      // Remove formatting guidelines from below the comments.
      $element['format']['guidelines'] = FALSE;
      $element['format']['format']['#access'] = FALSE;
      $element['format']['help']['#access'] = FALSE;
      $element['format']['#theme_wrappers'] = NULL;
    }
    else {
      $node_type = $element['#bundle'];
    }
    $field_name = $element['#field_name'];
  }
  // Webforms do not have a #bundle and #field_name element.
  // Look for elemend id instead.
  elseif (in_array('webform', arg())) {
    $node_type = 'webform';
    $field_name = $element['#id'];
  }
  
  if (isset($text_format_defaults[$node_type][$field_name])) {
    $default_format = $text_format_defaults[$node_type][$field_name];
    $element['format']['format']['#default_value'] = $default_format;
    
    // Go through all available text format options and remove it except for the default.
    // This is to prevent editors from picking other formats.
    foreach ($element['format']['format']['#options'] as $key => $value) {
      if ($key != $default_format) {
        unset($element['format']['format']['#options'][$key]);
      }
    }
  }
  
  return $element;
}

/**
 * Implements hook_form_FILE_ENTITY_EDIT_alter().
 */
function gs_helper_form_file_entity_edit_alter(&$form, &$form_state) {
  // we need to rewrite the form action, othwerise we end up on the homepage after submitting the form
  // we also add a 'cancel' button to close the modal
  if (preg_match('/render=media-popup$/', $form['#action']) && $fid = $form['fid']['#value']) {
    $base = '/file/' . $fid . '/edit';
    $form['#action'] = $base . '?destination=' . $base . '?render=media-popup';
    
    // hide the delete button
    unset($form['actions']['delete']);
    
    $form['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => '',
      '#attributes' => array(
        'class' => array('button', 'fake-cancel')
      ),
      '#weight' => 15,
    );
    
    $form['#suffix'] = <<<JS
<script>
  (function ($) {
  
  Drupal.behaviors.media_file = {
    attach: function (context, settings) {
        
      var dialog = $(parent.window.document.body).find('.ui-dialog');
      var overlay = $(parent.window.document.body).find('.ui-widget-overlay');  
      
      $('.edit', context).hide();
      
      function delayedAction() {
        console.log('delayedAction triggered');
        $("body").unbind("DOMSubtreeModified");
        clearTimeout(t);
        dialog.dialog("destroy");
        dialog.remove();
        overlay.remove();
      }
      
      $("body").bind("DOMSubtreeModified", function(){
        console.log('DOMSubtreeModified triggered');
        if ($(this).has(".status").length) {
          t = setTimeout(delayedAction, 500);
        }
        $('.edit', context).hide();
      });
      
      $('a.button.fake-cancel', context).once().bind('click', function() {
        console.log('cancel triggered');
        $("body").unbind("DOMSubtreeModified");
        dialog.dialog("destroy");
        dialog.remove();
        overlay.remove();
        return false;
      });
      
      $('#file-entity-edit input#edit-submit', context).once().bind('click',function(){
        console.log('save triggered');
        $(this).val('Processing...');
      });
      
    }
  };
  
  })(jQuery);
</script>
JS;

  }
}

/**
 * Implements hook_node_submit().
 */
function gs_helper_node_submit($node, $form, &$form_state) {
  if (isset($form_state['field']['field_mediacollection'], $form_state['field']['field_mediacollection'][LANGUAGE_NONE]['entity'])) {
    module_load_include('inc', 'gs_helper', 'gs_helper.images');
    
    foreach ($form_state['field']['field_mediacollection'][LANGUAGE_NONE]['entity'] as $media) {
      $crop_data = gs_helper_focus_crop_image($media, 'article_sidebar_thumbnail');
      
      if (isset($media->field_crop_data[LANGUAGE_NONE]) && $crop_data) {
        $media->field_crop_data[LANGUAGE_NONE][0]['value'] = serialize($crop_data);
      }
    }
  }
}

/**
 * Implements hook_field_attach_form().
 */
function gs_helper_field_attach_form($entity_type, $entity, &$form, &$form_state, $langcode) {
  // Specific settings for mediacollection multivalue node edit form
  if (isset($form['field_mediacollection'])) {
    // Check if we are not in an ajax callback
    // Add check for #max_delta so when there is only 1 empty entity, it does not get removed
    if (!isset($form_state['triggering_element']) && isset($form['field_mediacollection']['und']['#max_delta'])) {
      $element = &$form['field_mediacollection']['und'];
      _gs_helper_remove_items($element, 1, 'field_mediacollection', 'und', $form_state);
    }
  }
}
/**
 * Helper function - removes items from the given element
 */
function _gs_helper_remove_items(&$element, $remove, $field_name, $langcode, &$form_state) {
  foreach (range(1, $remove) as $i) {
    _gs_helper_remove_item($element, $field_name, $langcode, $form_state);
  }
}
/**
 * Helper function - removes one item from the given element
 */
function _gs_helper_remove_item(&$element, $field_name, $langcode, &$form_state) {
  $max_delta = $element['#max_delta'];
  unset($element[$max_delta]);
  $delta  = $max_delta - 1;
  $element['#max_delta'] = $delta;
  $element['#prefix'] = '<div id="field-test-add-more-wrapper' . '--' . $delta . '">';
  $element['add_more']['#ajax']['wrapper'] = 'field-test-add-more-wrapper' . '--' . $delta ;
  $form_state['field'][$field_name][$langcode]['items_count'] = $delta;
}
/**
 * Helper function - adds items to the given element
 */
function _gs_helper_add_items(&$element, $add, $field_name, $langcode, &$form_state) {
  foreach (range(1, $add) as $i) {
    _gs_helper_add_item($element, $field_name, $langcode, $form_state);
  }
}
/**
 * Helper function - adds one item to the given element
 */
function _gs_helper_add_item(&$element, $field_name, $langcode, &$form_state) {
  $old_delta = $element['#max_delta'];
  $delta = $old_delta + 1;
  // initialize new item
  $element[$delta] = $element[$old_delta];
  // incremenet item settings
  $element[$delta]['#delta'] = $delta;
  $element[$delta]['value']['#delta'] = $delta;
  $element[$delta]['#weight'] = $delta;
  $element[$delta]['value']['#weight'] = $delta;
  $element[$delta]['_weight']['#delta'] = $delta;
  $element[$delta]['_weight']['#default_value'] = $delta;
  // incremenet element settings
  $element['#max_delta'] = $delta;
  //$element['#prefix'] = '<div id="field-test-add-more-wrapper' . '--' . $i . '">';
  //$element['add_more']['#ajax']['wrapper'] = 'field-test-add-more-wrapper' . '--' . $i;
  // update the item count. Necessary for add_more ajax to work
  $form_state['field'][$field_name][$langcode]['items_count'] = $delta;
}
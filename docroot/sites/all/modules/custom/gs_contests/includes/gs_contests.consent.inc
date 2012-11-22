<?php

/**
 * @file
 * Contest consent component.
 */

// We treat select as a sort of "parent class"
//module_load_include('inc', 'webform', 'components/select');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_consent() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'mandatory' => 0,
    'pid' => 0,
    'weight' => 0,
    'value' => 0,
    'extra' => array(
      'condition' => 'By checking this box, I authorize Straight.com to email me information about upcoming promotions and contests.',
      'title_display' => 'none',
      'private' => FALSE,
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_consent() {
  return array(
    'webform_display_consent' => array(
      'render element' => 'element',
    ),
  );
}

/**
 * Format the text output for this component.
 */
function theme_webform_display_consent($variables) {
  $element = $variables['element'];
  $component = $element['#webform_component'];
  
  $options = array();
  foreach ($element['#options'] as $key => $value) {
    if (is_array($value)) {
      foreach ($value as $subkey => $subvalue) {
        $options[$subkey] = $subvalue;
      }
    }
    else {
      $options[$key] = $value;
    }
  }
  
  $items = array();
  
  if (isset($element['#value'][0]) && $element['#value'][0] !== '') {
    // Administer provided values.
    if (isset($options[$element['#value'][0]])) {
      $items[] = $element['#format'] == 'html' ? _webform_filter_xss($options[$element['#value'][0]]) : $options[$element['#value'][0]];
    }
    // User-specified in the "other" field.
    else {
      $items[] = $element['#format'] == 'html' ? check_plain($element['#value'][0]) : $element['#value'][0];
    }
  }
  
  if ($element['#format'] == 'html') {
    $output = count($items) > 1 ? theme('item_list', array('items' => $items)) : (isset($items[0]) ? $items[0] : ' ');
  }
  else {
    if (count($items) > 1) {
      foreach ($items as $key => $item) {
        $items[$key] = ' - ' . $item;
      }
      $output = implode("\n", $items);
    }
    else {
      $output = isset($items[0]) ? $items[0] : ' ';
    }
  }
  
  return $output;
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_consent($component) {
  $form['value'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this option if the consent field should be checked by default.'),
    '#default_value' => $component['value'],
    '#weight' => 0,
  );
  $form['extra']['condition'] = array(
    '#type' => 'textarea',
    '#title' => t('Consent Text'),
    '#default_value' => $component['extra']['condition'],
    '#description' => t(''),
    '#cols' => 60,
    '#rows' => 1,
    '#weight' => 1,
    '#required' => TRUE,
    '#wysiwyg' => FALSE,
  );
  
  return $form;
}

/**
 * Element validation callback. Ensure keys are not duplicated.
 */
function _webform_edit_validate_consent($element, &$form_state) {
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_consent($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  
  $element = array(
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#theme_wrappers' => array('checkboxes', 'webform_element'),
    '#pre_render' => array(), // Needed to disable double-wrapping of radios and checkboxes.
    '#translatable' => array('title'),
    '#type' => 'checkboxes',
    '#process' => array('webform_expand_checkboxes'),
  );
  
  $default_value = $filter ? _webform_filter_values($component['value'], $node, NULL, NULL, FALSE) : $component['value'];
  
  if (isset($value)) {
    // Set the value as a single string.
    $element['#default_value'] = '';
    foreach ((array) $value as $option_value) {
      $element['#default_value'][] = $option_value;
    }
  }
  elseif (isset($default_value)) {
    $element['#default_value'][] = $default_value;
  }
  else {
    $element['#default_value'][] = 0;
  }
  
  $element['#options'][1] = $component['extra']['condition'];
  
  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_consent($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_consent',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#format' => $format,
    '#options' => array(1 => $component['extra']['condition']),
    '#value' => (array) $value,
    '#translatable' => array('title', 'options'),
  );
}

/**
 * Implements _webform_submit_component().
 */
function _webform_submit_consent($component, $value) {
  $return = array();
  
  if (isset($value[1])) {
    $return[] = $value[1];
  }
  else {
    $return[] = 0;
  }

  return $return;
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_consent($component, $sids = array(), $single = FALSE) {
  $sid_placeholders = count($sids) ? array_fill(0, count($sids), "'%s'") : array();
  $sid_filter = count($sids) ? " AND sid IN (" . implode(",", $sid_placeholders) . ")" : "";
  
  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid'])
    ->condition('data', 1, '=')
    ->groupBy('data');
  $query->addExpression('COUNT(data)', 'datacount');

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  $result = $query->execute();
  $rows = array();
  $normal_count = 0;
  
  foreach ($result as $key => $data) {
    $rows[$data['data']] = array($component['extra']['condition'], $data['datacount']);
    $normal_count += $data['datacount'];
  }
  
  return $rows;
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_consent($component, $value) {
  return !isset($value[0]) ? '' : $value[0];
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_consent($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_consent($component, $export_options, $value) {
  return !isset($value[0]) ? '' : $value[0];
}
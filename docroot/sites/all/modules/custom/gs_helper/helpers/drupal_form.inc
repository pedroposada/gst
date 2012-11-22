<?php
/**
 * @file
 * Contains helper code for Form elements.
 */

/**
 * Wraps form item in an HTML div using prefix & suffix properties;
 * @return 
 * @param object $item
 */
function hide_form_item(&$item) {
  if ($item) {
    $item['#prefix']  = '<div class="hidden">';
    $item['#suffix']  = '</div>';
  }
}

/**
 * Moves specified $field_name to the $fieldset
 * @return 
 * @param object $form
 * @param object $fieldset
 */
function move_field_to_fieldset(&$form, &$fieldset, $field_name) {
  if (isset($form[$field_name])) {
    $fieldset[$field_name] = $form[$field_name];
    unset($form[$field_name]);
  }
}
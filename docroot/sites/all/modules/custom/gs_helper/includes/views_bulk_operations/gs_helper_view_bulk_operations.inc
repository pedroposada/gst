<?php

/**
 * Implements hook_views_bulk_operations_form_alter()
 */
function gs_helper_views_bulk_operations_form_alter(&$form, &$form_state, $vbo) {
  $form_ids = array(
    'views_form_all_content_page_1', 
    'views_form_all_content_page_2', 
    'views_form_all_content_page_3',
    'views_form_all_content_page_4',
    'views_form_manage_comments_page',
    'views_form_workbench_people_page',
  );
  // Check if Media (page) of Workbench Custom Tab view.
  if (in_array($form['#form_id'], $form_ids)) {
    $action = '';
    
    if (isset($form['select']['submit']['#submit'])) {
      $action = 'select';
    }
    
    if (isset($form['actions']['submit']['#submit'])) {
      $action = 'actions';
    }
    
    if (!empty($action)) {
      $key = array_search('views_bulk_operations_form_submit', $form[$action]['submit']['#submit']);
      if ($key !== FALSE) {
        // Use custom submit handler.
        $form[$action]['submit']['#submit'][$key] = 'gs_helper_bulk_operations_form_submit';
      }
    }
  }
}

/**
 * Submit handler for all steps of the VBO multistep form.
 */
function gs_helper_bulk_operations_form_submit($form, &$form_state) {
  views_bulk_operations_form_submit($form, $form_state);
  
  // Use custom confirm form handler.
  switch ($form_state['step']) {
    case 'views_bulk_operations_confirm_form':
      $form_state['step'] = 'gs_helper_bulk_operations_confirm_form';
      return;
      break;
    case 'gs_helper_bulk_operations_config_form':
      $form_state['step'] = 'gs_helper_bulk_operations_confirm_form';
      $operation = &$form_state['operation'];
      $operation->formSubmit($form, $form_state);

      if ($operation->getAdminOption('skip_confirmation')) {
        break; // Go directly to execution
      }
      $form_state['rebuild'] = TRUE;
      return;
      break;
  }
}

/**
 * Multistep form callback for the "configure" step.
 */
function gs_helper_bulk_operations_config_form($form, &$form_state, $view, $output) {
  $form = views_bulk_operations_config_form($form, $form_state, $view, $output);
  
  if (isset($form['actions']['submit']['#submit'])) {
    $key = array_search('views_bulk_operations_form_submit', $form['actions']['submit']['#submit']);
    if ($key !== FALSE) {
      // Use custom submit handler.
      $form['actions']['submit']['#submit'][$key] = 'gs_helper_bulk_operations_form_submit';
    }
  }
  
  return $form;
}

/**
 * Multistep form callback for the "confirm" step.
 */
function gs_helper_bulk_operations_confirm_form($form, &$form_state, $view, $output) {
  $form = views_bulk_operations_confirm_form($form, $form_state, $view, $output);
  $operation = '';
  
  switch ($form_state['operation']->operationId) {
    case 'action::views_bulk_operations_delete_item':
      $operation = 'Delete';
      break;
    case 'action::views_bulk_operations_modify_action':
      $operation = 'Modify';
      break;
    case 'action::nodequeue_add_action':
      $operation = 'Feature';
      break;
    case 'action::node_publish_action':
      $operation = 'Publish';
      break;
    case 'action::node_unpublish_action':
      $operation = 'Unpublish';
      break;
  }
  
  if (isset($form['description']['#markup']) && !empty($operation)) {
    $form['description']['#markup'] = str_replace('You selected the following' , $operation . ' the following', $form['description']['#markup']);
  }
  return $form;
}
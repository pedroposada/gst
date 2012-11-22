<?php

/**
 * Form to configure the download of CSV files.
 */
function contest_blacklist_form($form, &$form_state) {
  global $user;
  
  // Add blacklistee
  $form['add'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Add to blacklist'),
    '#attributes'    => array('class' => array('container-inline-formitem')),
    '#attached'      => array('css' => array(drupal_get_path('module', 'gs_contests') . '/css/gs_contests.blacklist.css')),
  );
  $form['add']['name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Full Name'),
    '#size'          => 24,
    '#maxlength'     => 255,
    '#default_value' => '',
  );
  $form['add']['email'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Email'),
    '#size'          => 24,
    '#maxlength'     => EMAIL_MAX_LENGTH,
  );
  $form['add']['phone'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Phone Number'),
    '#size'          => 24,
    '#maxlength'     => 255,
    '#default_value' => '',
  );
  $form['add']['reason'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Reason'),
    '#size'          => 48,
    '#maxlength'     => 255,
    '#default_value' => '',
  );
  $form['add']['duration'] = array(
    '#type'          => 'select',
    '#options'       => array(0 => 'Forever', 1 => '1 Month', 2 => '1 Year'),
    '#default_value' => 'Forever',
  );
  $form['add']['uid'] = array(
    '#type'          => 'value',
    '#value'         => $user->uid,
  );
  $form['add']['add'] = array(
    '#type'          => 'submit',
    '#value'         => t('Add'),
  );
  
  // Admin blacklist
  $header = array(
    'name'         => array('data' => t('Name'), 'field' => 'b.name'),
    'email'        => array('data' => t('Email'), 'field' => 'b.email'),
    'phone_number' => array('data' => t('Phone')),
    'reason'       => array('data' => t('Reason')),
    'submitted'    => array('data' => t('Added'), 'field' => 'b.submitted', 'sort' => 'desc'),
    'duration'     => array('data' => t('Duration'), 'field' => 'b.duration'),
  );
  
  $query = db_select('gs_contests_blacklist', 'b');
  $query = $query->extend('PagerDefault')->extend('TableSort');
  
  $count_query = clone $query;
  $count_query->addExpression('COUNT(b.bid)');
  
  $query
    ->fields('b', array('bid', 'name', 'email', 'phone_number', 'reason', 'submitted', 'duration'))
    ->limit(50)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();  
  
  $blacklist = array();
  
  $duration_text = array(
    0 => 'Forever',
    1 => '1 Month',
    2 => '1 Year',
  );
  
  foreach ($result as $blacklistee) {
    $blacklist[$blacklistee->bid] = array(
      'name'         => $blacklistee->name,
      'email'        => $blacklistee->email,
      'phone_number' => $blacklistee->phone_number,
      'reason'       => $blacklistee->reason,
      'submitted'    => date('F j, Y', $blacklistee->submitted),
      'duration'     => $duration_text[$blacklistee->duration],
    );
  }
  
  $form['blacklist'] = array(
    '#type'    => 'tableselect',
    '#header'  => $header,
    '#options' => $blacklist,
    '#empty'   => t('No people are blacklisted.'),
  );
  $form['delete'] = array(
    '#type'  => 'submit',
    '#value' => t('Delete Selected'),
  );
  $form['pager'] = array('#markup' => theme('pager'));
  
  return $form;
}

function contest_blacklist_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  
  switch ($op) {
    case 'Add':
      $values = $form_state['values'];
      $blacklist_submitted = time();
      $blacklist_end = 0;
      
      switch ($values['duration']) {
        case 1:
          $blacklist_end = strtotime('+1 month', $blacklist_submitted);
          break;
        case 2:
          $blacklist_end = strtotime('+1 year', $blacklist_submitted);
          break;
      }
      
      db_insert('gs_contests_blacklist')
        ->fields(array(
          'name'          => $values['name'],
          'email'         => $values['email'],
          'phone_number'  => $values['phone'],
          'reason'        => check_plain($values['reason']),
          'submitted'     => $blacklist_submitted,
          'uid'           => $values['uid'],
          'duration'      => $values['duration'],
          'blacklist_end' => $blacklist_end,
        ))
        ->execute();
      break;
    case 'Delete Selected':
      $to_delete = array_filter($form_state['values']['blacklist']);
      db_delete('gs_contests_blacklist')
        ->condition('bid', $to_delete, 'IN')
        ->execute();
      drupal_set_message(t('Delete has been performed.'));
      break;
  }
}

function contest_blacklist_form_validate($form, &$form_state) {
  $op = $form_state['values']['op'];
  
  switch ($op) {
    case 'Add':
      $email = $form_state['values']['email'];
      $phone = trim($form_state['values']['phone']);
      
      if (!$email) {
        form_set_error('email', t('You must at least enter an email address.'));
      }
      if (!valid_email_address($email)) {
        form_set_error('email', t('"%email" is not a valid email address.', array('%email' => $email)));
      }
      if ($phone) {
        if (preg_match('/^(?:\+?1[-. ]?)?\(?(\d{3})\)?[-. ]?(\d{3})[-. ]?(\d{4})[-. ]?(?:(?:x|ext) ?(\d+))?$/i', $phone, $matches)) {
          $form_state['values']['phone'] = '(' . $matches[1] . ') ' . $matches[2] . '-' . $matches[3] . (isset($matches[4]) ? ' x' . $matches[4] : '');
        }
        else {
          form_set_error('phone', t('"%phone" is not a valid phone number. Please enter a valid ten-digit phone number with optional extension. Example: 604-555-5555 x123', array('%phone' => $phone)));
        }
      }
      break;
    case 'Delete Selected':
      $form_state['values']['blacklist'] = array_filter($form_state['values']['blacklist']);
      if (count($form_state['values']['blacklist']) == 0) {
        form_set_error('', t('Noone selected to delete.'));
      }
      break;
  }
}
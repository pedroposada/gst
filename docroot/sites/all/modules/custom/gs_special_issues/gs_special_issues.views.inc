<?php

/**
 * Implementation hook_views_data()
 */
function gs_special_issues_views_data() {
  // The 'group' index will be used as a prefix in the UI for any of this
  // table's fields, sort criteria, etc. so it's easy to tell where they came
  // from.
  $data['gs_special_issues_bov_winners']['table']['group'] = t('BoV');
  
  // Define this as a base table. In reality this is not very useful for
  // this table, as it isn't really a distinct object of its own, but
  // it makes a good example.
  $data['gs_special_issues_bov_winners']['table']['base'] = array(
    'field' => 'wid',
    'title' => t('BoV Winners'),
    'help' => t("BoV Winners table contains winners from Best of Vancouver."),
    'weight' => -10,
  );
  
  // This table references the {gs_special_issues_bov_categories} table.
  // This creates an 'implicit' relationship to the gs_special_issues_bov_winners table, so that
  // the fields are automatically available.
  //$data['example_table']['table']['join'] = array(
    // Index this array by the table name to which this table refers.
    // 'left_field' is the primary key in the referenced table.
    // 'field' is the foreign key in this table.
    //'gs_special_issues_bov_categories' => array(
    //  'left_field' => 'category',
    //  'field' => 'cid',
    //),
  //);
  
  // Next, describe each of the individual fields in this table to Views. For
  // each field, you may define what field, sort, argument, and/or filter
  // handlers it supports. This will determine where in the Views interface you
  // may use the field.
  
  // Example plain text field.
  $data['gs_special_issues_bov_winners']['name'] = array(
    'title' => t('Winner\'s Name'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  $data['gs_special_issues_bov_winners']['address'] = array(
    'title' => t('Winner\'s Address'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  $data['gs_special_issues_bov_winners']['phone'] = array(
    'title' => t('Winner\'s Phone'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  $data['gs_special_issues_bov_winners']['link'] = array(
    'title' => t('Winner\'s Link'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  $data['gs_special_issues_bov_winners']['year'] = array(
    'title' => t('Winner\'s Year'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['gs_special_issues_bov_winners']['category'] = array(
    'title' => t('Winner\'s Year'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['gs_special_issues_bov_winners']['group'] = array(
    'title' => t('Winner\'s Group'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  return $data;
}
<?php
/***************************************************************************************
 *          Front page headlines admin form and handlers (form, validate, submit and ajax)
 ***************************************************************************************/
function gs_helper_news_rotator_form($form, &$form_state) {
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="nr-form-wrapper">';
  $form['#suffix'] = '</div>';
  // preview pane
  $tabsvals = variable_get('gs_helper_news_rotator_values');
  $form['preview_pane'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#weight' => -100,
    '#value' => theme('gs_helper_front_headlines',array('items'=>$tabsvals)),
  );
  // get current number of headlines
  $num = isset($tabsvals) ? count($tabsvals) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add headlines
  $i = 0;
  do {
    $element["headline"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['headline'] : '',
      '#description' => t('Enter the title of the link.'),
    );
    $element["url"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['url'] : '',
      '#description' => t('Enter link in the form of "http://www.mylink.com"'),
      '#maxlength' => 600,
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'gs_helper_news_rotator_form_ajax',
        'wrapper' => 'nr-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['headlines'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'gs_helper_news_rotator_form_ajax',
      'wrapper' => 'nr-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['preview'] = array(
    '#type'=>'submit',
    '#value'=>t('Preview'),
    '#ajax' => array(
      'callback' => 'gs_helper_news_rotator_form_ajax',
      'wrapper' => 'nr-form-wrapper',
    ),
    '#name' => 'preview',
    '#weight' => 101,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'gs_helper_news_rotator_form_ajax',
      'wrapper' => 'nr-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function gs_helper_news_rotator_form_validate($form, &$form_state) {
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['headlines']) < 2) {
      form_set_error('',"Cannot remove this item. The minimum number of items in this list is 1.");
    }
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $keys = array();
    foreach ($form_state['values']['headlines'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['headline'])) {
          form_error($form['headlines'][$key]['headline'], "Title is required.");
        }
        if (!valid_url($value['url'], TRUE)) {
          form_error($form['headlines'][$key]['url'], "Invalid Link.");
        }
      }
    }
  }
}
function gs_helper_news_rotator_form_submit($form, &$form_state) {
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    $removed = $form_state['triggering_element']['#array_parents'][1];
    $form_state['storage']['remove'][] = $removed;
    drupal_set_message(t("Changes made in this table will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $items = $weights = $values = array();
    foreach ($form_state['values']['headlines'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[] = $value;
        $weights[] = $value['weight'];
      }
    }
    // array_multisort($weights, $items);
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_front_headlines',array('items'=>$values));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('gs_helper_news_rotator_values', $values);
    drupal_set_message("Headlines were saved successfully.");
    drupal_set_message(t("Items are now live on the !frontpage.",array('!frontpage'=>l('front-page',''))));
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function gs_helper_news_rotator_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['headlines']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['headlines'][$key]);
    }
  }
  // preview
  if (isset($form_state['storage']['preview_pane'])) {
    $form['preview_pane']['#value'] = $form_state['storage']['preview_pane'];
  }
  return $form;
}
/************************************************************************************
 *            Product detail page tabs block admin form (form, validate, submit and ajax)
 ************************************************************************************/
function gs_helper_prod_tabs_form($form, &$form_state) {
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="prod-tabs-form-wrapper">';
  $form['#suffix'] = '</div>';
  // empty cart message
  $form['message'] = array(
    '#type' =>  'fieldset', 
  );
  $form['message']['empty_cart_message'] = array(
    '#type' => 'text_format',
    '#title' => t('Empty Cart Message'),
    '#format' => 'full_html',
    '#default_value' => variable_get('gs_helper_empty_cart_message'),
  );  
  $form['message']['empty_cart_save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Message'),
    '#ajax' => array(
      'callback' => 'gs_helper_prod_tabs_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'empty_cart_save_message',
  );
  // preview pane
  $tabsvals = variable_get('gs_helper_prod_tabs_values');
  $form['preview_pane'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#weight' => -100,
    '#value' => theme('gs_helper_prod_tabs',array('items'=>$tabsvals,'style'=>'Zen')),
  );
  // get current number of headlines
  $num = isset($tabsvals) ? count($tabsvals) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add items
  $i = 0;
  do {
    $element["tabtitle"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabtitle'] : '',
      '#size'=>30,
      '#description' => t('Enter the title of the tab.'),
    );
    $element["tabdescription"] = array(
      '#type' => 'text_format',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabdescription']['value'] : '',
      '#format' => 'full_html',
      '#maxlength' => 600,
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'gs_helper_prod_tabs_form_ajax',
        'wrapper' => 'prod-tabs-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['tabs'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'gs_helper_prod_tabs_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['preview'] = array(
    '#type'=>'submit',
    '#value'=>t('Preview'),
    '#ajax' => array(
      'callback' => 'gs_helper_prod_tabs_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'preview',
    '#weight' => 101,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'gs_helper_prod_tabs_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function gs_helper_prod_tabs_form_validate($form, &$form_state) {
  // empty cart save message
  if ($form_state['triggering_element']['#name'] == 'empty_cart_save_message') {
    if (empty($form_state['values']['message']['empty_cart_message']['value'])) {
      form_set_error($form['message']['empty_cart_message']['value'],"Invalid empty cart message.");
    }
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['tabs']) < 2) {
      form_set_error('',"Cannot remove this item. The minimum number of items in this list is 1.");
    }
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $keys = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['tabtitle'])) {
          form_error($form['tabs'][$key]['tabtitle'], "Title is required.");
        }
        if (empty($value['tabdescription']['value'])) {
          form_error($form['tabs'][$key]['tabdescription']['value'], "Description is required.");
        }
      }
    }
  }
}
function gs_helper_prod_tabs_form_submit($form, &$form_state) {
  // save empty cart message
  if ($form_state['triggering_element']['#name'] == 'empty_cart_save_message') {
    variable_set('gs_helper_empty_cart_message', $form_state['values']['message']['empty_cart_message']['value']);
    drupal_set_message('Message saved.');
  }
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    $removed = $form_state['triggering_element']['#array_parents'][1];
    $form_state['storage']['remove'][] = $removed;
    drupal_set_message(t("Changes made in this table will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $weights = $values = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[] = $value;
        $weights[] = $value['weight'];
      }
    }
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_prod_tabs',array('items'=>$values,'style'=>'Zen'));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('gs_helper_prod_tabs_values', $values);
    drupal_set_message("Tabs were saved successfully.");
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function gs_helper_prod_tabs_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['tabs']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['tabs'][$key]);
    }
  }
  // preview
  if (isset($form_state['storage']['preview_pane'])) {
    $form['preview_pane']['#value'] = $form_state['storage']['preview_pane'];
  }
  return $form;
}
/******************************************************************************************
 *            LGBT external links admin form (form, validate, submit and ajax)
 ******************************************************************************************/
function gs_helper_lgbt_external_links_form($form, &$form_state) {
  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#value' => t("This content is visible at !link",array('!link'=>l('life/lgbt','life/lgbt')))
  );
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="prod-tabs-form-wrapper">';
  $form['#suffix'] = '</div>';
  // preview pane
  $tabsvals = variable_get('gs_helper_lgbt_external_links');
  $form['preview_pane'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#weight' => -100,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => theme('gs_helper_lgbt_external_links',array('items'=>$tabsvals)),
  );
  // get current number of headlines
  $num = isset($tabsvals) ? count($tabsvals) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add items
  $i = 0;
  do {
    $element["tabtitle"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabtitle'] : '',
      '#description' => t('Enter the title of the link.'),
    );
    $element["tablink"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tablink'] : '',
      '#description' => t('Enter link in the form of http://www.mylink.com/'),
      '#maxlength' => 600,
    );
    $element["tabsource"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabsource'] : '',
      '#description' => t('Enter the name of the website where this links to.'),
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'gs_helper_lgbt_external_links_form_ajax',
        'wrapper' => 'prod-tabs-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['tabs'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'gs_helper_lgbt_external_links_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['preview'] = array(
    '#type'=>'submit',
    '#value'=>t('Preview'),
    '#ajax' => array(
      'callback' => 'gs_helper_lgbt_external_links_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'preview',
    '#weight' => 101,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'gs_helper_lgbt_external_links_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function gs_helper_lgbt_external_links_form_validate($form, &$form_state) {
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['tabs']) < 2) {
      form_set_error('',"Cannot remove this item. The minimum number of items in this list is 1.");
    }
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $keys = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['tabtitle'])) {
          form_error($form['tabs'][$key]['tabtitle'], "Title is required.");
        }
        if (empty($value['tablink']) || !valid_url($value['tablink'], TRUE)) {
          form_error($form['tabs'][$key]['tablink'], "Invalid Link.");
        }
        if (empty($value['tabsource'])) {
          form_error($form['tabs'][$key]['tabsource'], "Source page is required.");
        }
      }
    }
  }
}
function gs_helper_lgbt_external_links_form_submit($form, &$form_state) {
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    $removed = $form_state['triggering_element']['#array_parents'][1];
    $form_state['storage']['remove'][] = $removed;
    drupal_set_message(t("Changes made in this table will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $weights = $values = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[] = $value;
        $weights[] = $value['weight'];
      }
    }
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_lgbt_external_links',array('items'=>$values));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('gs_helper_lgbt_external_links', $values);
    drupal_set_message("Links were saved successfully.");
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function gs_helper_lgbt_external_links_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['tabs']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['tabs'][$key]);
    }
  }
  // preview
  if (isset($form_state['storage']['preview_pane'])) {
    $form['preview_pane']['#value'] = $form_state['storage']['preview_pane'];
  }
  return $form;
}
/****************************************************************************************************
 *              HOT TOPICS (form, validate, submit and ajax)
 ****************************************************************************************************/
function gs_helper_hot_topics_form($form, &$form_state) {
  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#value' => t("This content is visible at !link",array('!link'=>l('news','news'))),
  );
  $form['#theme'] = 'gs_helper_lgbt_external_links_form';
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="prod-tabs-form-wrapper">';
  $form['#suffix'] = '</div>';
  // preview pane
  $tabsvals = variable_get('gs_helper_hot_topics');
  $form['preview_pane'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#weight' => -100,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => theme('gs_helper_lgbt_external_links',array('items'=>$tabsvals)),
  );
  // get current number of headlines
  $num = isset($tabsvals) ? count($tabsvals) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add items
  $i = 0;
  do {
    $element["tabtitle"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabtitle'] : '',
      '#description' => t('Enter the title of the link.'),
    );
    $element["tablink"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tablink'] : '',
      '#description' => t('Enter link in the form of http://www.mylink.com/'),
      '#maxlength' => 600,
    );
    $element["tabsource"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabsource'] : '',
      '#description' => t('Enter the name of the website where this links to.'),
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'gs_helper_hot_topics_form_ajax',
        'wrapper' => 'prod-tabs-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['tabs'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'gs_helper_hot_topics_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['preview'] = array(
    '#type'=>'submit',
    '#value'=>t('Preview'),
    '#ajax' => array(
      'callback' => 'gs_helper_hot_topics_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'preview',
    '#weight' => 101,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'gs_helper_hot_topics_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function gs_helper_hot_topics_form_validate($form, &$form_state) {
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['tabs']) < 2) {
      form_set_error('',"Cannot remove this item. The minimum number of items in this list is 1.");
    }
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $keys = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['tabtitle'])) {
          form_error($form['tabs'][$key]['tabtitle'], "Title is required.");
        }
        if (empty($value['tablink']) || !valid_url($value['tablink'], TRUE)) {
          form_error($form['tabs'][$key]['tablink'], "Invalid Link.");
        }
        if (empty($value['tabsource'])) {
          form_error($form['tabs'][$key]['tabsource'], "Source page is required.");
        }
      }
    }
  }
}
function gs_helper_hot_topics_form_submit($form, &$form_state) {
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    $removed = $form_state['triggering_element']['#array_parents'][1];
    $form_state['storage']['remove'][] = $removed;
    drupal_set_message(t("Changes made in this table will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $weights = $values = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[] = $value;
        $weights[] = $value['weight'];
      }
    }
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_lgbt_external_links',array('items'=>$values));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('gs_helper_hot_topics', $values);
    drupal_set_message("Links were saved successfully.");
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function gs_helper_hot_topics_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['tabs']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['tabs'][$key]);
    }
  }
  // preview
  if (isset($form_state['storage']['preview_pane'])) {
    $form['preview_pane']['#value'] = $form_state['storage']['preview_pane'];
  }
  return $form;
}
/****************************************************************************************************
 *              TECH & GAMES (form, validate, submit and ajax)
 ****************************************************************************************************/
function gs_helper_techgames_form($form, &$form_state) {
  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#value' => t("This content is visible at !link",array('!link'=>l('life/tech-games','life/tech-games'))),
  );
  $form['#theme'] = 'gs_helper_lgbt_external_links_form';
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="prod-tabs-form-wrapper">';
  $form['#suffix'] = '</div>';
  // preview pane
  $tabsvals = variable_get('gs_helper_techgames');
  $form['preview_pane'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#weight' => -100,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => theme('gs_helper_lgbt_external_links',array('items'=>$tabsvals)),
  );
  // get current number of headlines
  $num = isset($tabsvals) ? count($tabsvals) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add items
  $i = 0;
  do {
    $element["tabtitle"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabtitle'] : '',
      '#description' => t('Enter the title of the link.'),
    );
    $element["tablink"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tablink'] : '',
      '#description' => t('Enter link in the form of http://www.mylink.com/'),
      '#maxlength' => 600,
    );
    $element["tabsource"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['tabsource'] : '',
      '#description' => t('Enter the name of the website where this links to.'),
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'gs_helper_hot_topics_form_ajax',
        'wrapper' => 'prod-tabs-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['tabs'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'gs_helper_hot_topics_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['preview'] = array(
    '#type'=>'submit',
    '#value'=>t('Preview'),
    '#ajax' => array(
      'callback' => 'gs_helper_hot_topics_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'preview',
    '#weight' => 101,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'gs_helper_hot_topics_form_ajax',
      'wrapper' => 'prod-tabs-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function gs_helper_techgames_form_validate($form, &$form_state) {
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['tabs']) < 2) {
      form_set_error('',"Cannot remove this item. The minimum number of items in this list is 1.");
    }
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $keys = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['tabtitle'])) {
          form_error($form['tabs'][$key]['tabtitle'], "Title is required.");
        }
        if (empty($value['tablink']) || !valid_url($value['tablink'], TRUE)) {
          form_error($form['tabs'][$key]['tablink'], "Invalid Link.");
        }
        if (empty($value['tabsource'])) {
          form_error($form['tabs'][$key]['tabsource'], "Source page is required.");
        }
      }
    }
  }
}
function gs_helper_techgames_form_submit($form, &$form_state) {
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    $removed = $form_state['triggering_element']['#array_parents'][1];
    $form_state['storage']['remove'][] = $removed;
    drupal_set_message(t("Changes made in this table will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $weights = $values = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[] = $value;
        $weights[] = $value['weight'];
      }
    }
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_lgbt_external_links',array('items'=>$values));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('gs_helper_techgames', $values);
    drupal_set_message("Links were saved successfully.");
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function gs_helper_techgames_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['tabs']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['tabs'][$key]);
    }
  }
  // preview
  if (isset($form_state['storage']['preview_pane'])) {
    $form['preview_pane']['#value'] = $form_state['storage']['preview_pane'];
  }
  return $form;
}
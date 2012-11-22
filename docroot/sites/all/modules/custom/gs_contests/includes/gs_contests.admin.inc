<?php

/**
 * Menu callback for admin/content/webform. Displays all webforms on the site.
 */
function gs_contests_admin_content() {
  $output = '';
  
  $contests = db_select('node', 'n')
    ->fields('n')
    ->condition('n.type', 'contest')
    ->extend('PagerDefault')
    ->limit(25)
    ->execute()
    ->fetchAllAssoc('nid');
  
  $output .= theme('gs_contests_admin_content', array('node' => $contests));
  $output .= theme('pager', array('tags' => array()));
  
  return $output;
}

/**
 * Generate a list of all webforms avaliable on this site.
 */
function theme_gs_contests_admin_content($variables) {
  $nodes = $variables['node'];
  
  $header = array(
    t('Contests'),
    t('Status'),
    array('data' => t('View'), 'colspan' => '4'),
    array('data' => t('Operations'), 'colspan' => '4')
  );

  $rows = array();
  foreach ($nodes as $node) {
    $rows[] = array(
      l($node->title, 'node/' . $node->nid),
      $node->status ? t('Published') : t('Unpublished'),
      l(t('Submissions'), 'node/' . $node->nid . '/webform-results'),
      l(t('Analysis'), 'node/' . $node->nid . '/webform-results/analysis'),
      l(t('Table'), 'node/' . $node->nid . '/webform-results/table'),
      l(t('Download'), 'node/' . $node->nid . '/webform-results/download'),
      node_access('update', $node) ? l(t('Edit'), 'node/' . $node->nid . '/edit') : '',
      node_access('update', $node) ? l(t('Webform'), 'node/' . $node->nid . '/webform') : '',
      l(t('Draw'), 'node/' . $node->nid . '/webform-results/draw'),
      user_access('delete all webform submissions') ? l(t('Clear'), 'node/' . $node->nid . '/webform-results/clear') : '',
    );
  }

  if (empty($rows)) {
    $webform_types = webform_variable_get('webform_node_types');
    if (empty($webform_types)) {
      $message = t('Webform is currently not enabled on any content types.') . ' ' . t('Visit the <a href="!url">Webform settings</a> page and enable Webform on at least one content type.', array('!url' => url('admin/config/content/webform')));
    }
    else {
      $webform_type_list = webform_admin_type_list();
      $message = t('There are currently no webforms on your site. Create a !types piece of content.', array('!types' => $webform_type_list));
    }

    $rows[] = array(
      array('data' => $message, 'colspan' => 8),
    );
  }
  
  $output = '<ul class="action-links"><li><a href="/node/add/contest">Create Contest</a></li></ul>';
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  
  return $output;
}

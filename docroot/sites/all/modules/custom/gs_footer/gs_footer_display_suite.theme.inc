<?php

/**
 * @file
 * Contains code specific to Display Suite's theming functionality.
 */
function theme_gs_footer_issue_articles($vars) {
  include_once(drupal_get_path('module', 'gs_footer') . '/includes/gs_footer_sitemap.inc');
  
  $issue_time = gs_footer_get_unix_timestamp($vars['entity']->field_issue_date['und'][0]['value']);

  $results = db_query("SELECT * FROM {node} WHERE (created > :min_issue_time AND created < :max_issue_time) AND (type = :article OR type = :blog)", array(':article' => 'article', ':blog' => 'blog_post', ':min_issue_time' => $issue_time - GS_ISSUE_LOWER_TIME, ':max_issue_time' => $issue_time + GS_ISSUE_OVER_TIME));
  $info = array(); $taxonomy = array(); $order = array(); $output = ''; $section_order =array();

  foreach ($results as $record) {
    $node = node_load($record->nid);
    $tax = new stdClass();

    if($node->type == 'article') {
      $tax = taxonomy_term_load($node->field_section['und'][0]['tid']);
    }
    elseif ($node->type == 'blog_post') {
      $tax = taxonomy_term_load($node->field_blog_term['und'][0]['tid']);
    }
    
	// get _gs_footer_section_tid_map_search($inf['tid'], TRUE) or if empty get parent 
    $parent = gs_footer_get_tax_parents($tax->tid);
	$section = _gs_footer_section_tid_map_search($parent, TRUE);

    $info[] = array('nid' => $node->nid, 'title' => $node->title, 'tid' => $tax->tid, 'taxname' => $tax->name, 'section' => $section);
    $taxonomy[] = array('tid' => $tax->tid, 'name' => $tax->name);

    if(!in_array($tax->name, $order)) {
      array_push($order, $tax->name);
    }
    if(!in_array($section, $section_order)) {
      array_push($section_order, $section);
    }
  }
  
  sort($order);
  
  usort($info, "gs_footer_cmp_taxname");
  usort($info, "gs_footer_cmp_section");

  foreach ($info as $key => $value) {
    if(!in_array($value['section'], $section_order)) {
      array_push($section_order, $value['section']);
    }
  }
  
  sort($section_order);
  
  $section_mem = '';
  foreach($info as $inf) {
	if ($section_mem != $inf['section']) {
      $output .= '<div id="issuepage-section-title">' . $inf['section'] . '</div>';
      $section_mem = $inf['section'];
	}
	if ($inf['section']) {
      $output .= '<div id="issuepage-taxonomy-title">' . l($inf['taxname'], 'taxonomy/term/' . $inf['tid'], array('attributes' => array('class' => array('issuepage-tax-term')))) . ' &raquo;</div>';
      $output .= l($inf['title'], 'node/' . $inf['nid'], array('attributes' => array('class' => array('issuepage-content'))));
	}
  }

  return $output;
}

/**
 * Sort array by field taxname
 */
function gs_footer_cmp_taxname($a, $b)
{
  return strcmp($a["taxname"], $b["taxname"]);
}

/**
 * Sort array by field section
 */
function gs_footer_cmp_section($a, $b)
{
  return strcmp($a["section"], $b["section"]);
}

/**
 * returns top parent item
 */
function gs_footer_get_tax_parents($tax = 0) {
  $tmp_terms = taxonomy_get_parents($tax); 
  $parent = array();
  foreach ($tmp_terms as $key => $value) {
	$parent = $value->tid;
  }  
  if(empty($parent)) {
    return $tax;
  }
  else {
    return gs_footer_get_tax_parents($parent);
  }
}

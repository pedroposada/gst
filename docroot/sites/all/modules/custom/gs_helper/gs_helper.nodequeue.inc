<?php

/**
 * @file
 * Contains code specific to the Nodequeue module.
 */

/**
 * Page callback for autocomplete.
 */
function gs_nodequeue_autocomplete() {
  $args = func_get_args();
  $sqid = array_shift($args);
  $string = implode('/', $args);
  $matches = _gs_nodequeue_autocomplete($sqid, $string);
  drupal_json_output(drupal_map_assoc($matches));
}

function _gs_nodequeue_autocomplete($sqid, $string) {
  $output = array();

  if (!is_numeric($sqid) || !$string) {
    return $output;
  }

  $subqueue = nodequeue_load_subqueue($sqid);
  if (!$subqueue) {
    return $output;
  }

  $queue = nodequeue_load($subqueue->qid);
  if (!$queue) {
    return $output;
  }

  $nodes = gs_nodequeue_api_autocomplete($queue, $subqueue, $string);
  return $nodes;
}

/**
 * Fetch a list of nodes available to a given subqueue
 * for autocomplete.
 *
 * @param $queue
 *   The queue that owns the subqueue
 * @param $subqueue
 *   The subqueue
 * @param $string
 *   The string being matched.
 *
 * @return
 *   An keyed array $nid => $title
 */
function gs_nodequeue_api_autocomplete($queue, $subqueue, $string) {
  $nodequeue_term_map = _gs_helper_nodequeue_term_map();
  
  $matches = array();
  if (empty($string)) {
    return $matches;
  }
    
  $query = new EntityFieldQuery();
  
  $query->entityCondition('entity_type', 'node');
  $query->propertyOrderBy('created', 'DESC');
  $query->addTag('node_access');
  $query->range(0, variable_get('nodequeue_autocomplete_limit', 10));

  if (!empty($queue->types)) {
    $query->entityCondition('bundle', $queue->types, 'IN');
  }

  $where_args = array();
  global $user;
  if (!user_access('administer nodes', $user)) {
    $query->propertyCondition('status', 1)->propertyCondition('uid', $user->uid);
  }

  // Run a match to see if they're specifying by nid.
  $preg_matches = array();
  $match = preg_match('/\[nid: (\d+)\]/', $string, $preg_matches);
  if (!$match) {
    $match = preg_match('/^nid: (\d+)/', $string, $preg_matches);
  }

  if ($match) {
    // If it found a nid via specification, reduce our resultset to just that nid.
    $query->propertyCondition('nid', $preg_matches[1]);
  }
  else {
    // Build the constant parts of the query.
    $query->propertyCondition('title', '%' . addcslashes($string, '\%_') . '%', 'LIKE');
  }
  
  if (!empty($queue->qid)) {
    $terms = array();
    
    foreach ($nodequeue_term_map[$queue->qid] as $tid) {
      $term = taxonomy_term_load($tid);
      $children = taxonomy_get_children($tid, $term->vid);
      
      foreach ($children as $child) {
        $terms[] = $child->tid;
      }
      
      $terms[] = $tid;
    }
    
    $query->fieldCondition('field_section', 'tid', $terms, 'IN');
  }

  // Call to the API.
  $function = $queue->owner . "_nodequeue_autocomplete";
  if (function_exists($function)) {
    return $function($queue, $subqueue, $string, $where, $where_args);
  }
  else {
    $query->addTag('i18n_select');
    $result = $query->execute();
    
    if (isset($result['node'])) {
      $nodes = array_keys($result['node']);
      $nodes = entity_load('node', $nodes);
    }

    foreach ($nodes as $node) {
      $id = nodequeue_get_content_id($queue, $node);
      $matches[$node->nid] = check_plain($node->title) . " [nid: $id]";
    }
  }

  return $matches;
}
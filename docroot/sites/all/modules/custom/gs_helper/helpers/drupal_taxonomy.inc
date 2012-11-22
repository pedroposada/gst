<?php
/**
 * @file
 * Contains helper code for Taxonomy.
 */

/**
 * Generates & returns a nested array of taxonomy terms for the specified vocabulary id;
 * @return 
 * @param object $vid
 * @param object $parent[optional]
 * @param object $max_depth[optional]
 */
function build_taxonomy_tree($vid, $parent = 0, $max_depth = NULL) {
  static $results;
  
  $args = func_get_args();
  $hash = md5(implode('-', $args));

  if (!isset($results[$hash])) {
    $terms = array();
    $data = taxonomy_get_tree($vid, $parent, $max_depth, FALSE);
    
    foreach ($data as $term) {
      $terms[$term->tid] = $term;
    }
    
    $terms = nest_taxonomy_terms($terms, $parent);
    
    $results[$hash] = $terms;
  }
    
  return $results[$hash];
}

/**
 * Looks for each term's parent term in the given tree and assigns 
 * the child to the parent.
 * @return 
 * @param object $terms
 * @param object $parent
 */
function nest_taxonomy_terms($terms, $parent) {
  foreach ($terms as $term) {
    $parent_tid = isset($term->parents[0]) ? $term->parents[0] : 0;

    if ($parent_tid) {
      if (isset($terms[$parent_tid])) {
        $terms[$parent_tid]->children[$term->tid] = $term;
        unset($terms[$term->tid]);
      }
      elseif ($parent_tid != $parent) {
        nest_taxonomy_terms_child($terms, $parent_tid, $term);
        unset($terms[$term->tid]);
      }
    }
  }
  
  return $terms;
}

/**
 * Recursive function which nests the child element within the appropriate
 * parent element in the given terms tree.
 * @return 
 * @param object $terms
 * @param object $parent_tid
 * @param object $child
 */
function nest_taxonomy_terms_child(&$terms, $parent_tid, $child) {
  foreach ($terms as &$term) {
    if ($term->tid == $parent_tid) {
      $term->children[$child->tid] = $child;
      break;
    }
    elseif (!empty($term->children)) {
      nest_taxonomy_terms_child($term->children, $parent_tid, $child);
    }
  }
}

/**
 * Converts a tree of taxonomy terms returned by build_taxonomy_tree() into a flat
 * array suitable for use as $options in form select elements.
 * @return 
 */
function tree_to_select_options($tree, $reset = TRUE) {
  static $options, $current_depth;
  
  if ($reset) {
    $options    = NULL;
    $current_depth  = NULL;
  }
  
  if (is_null($options)) {
    $options = array();
  }
    
  if (is_null($current_depth)) {
    $current_depth = 0;
  }
  else {  
    $current_depth++;
  }

  foreach ($tree as $term) {
    $prefix = '';
    
    for ($i = 0; $i < $current_depth * 2; $i++) {
      $prefix .= '-';
    }
  
    $options[$term->tid] = $prefix . $term->name;
    
    if (!empty($term->children)) {
      $options += tree_to_select_options($term->children, FALSE);
    }
  }
  
  $current_depth--;
  
  return $options;
}

/**
 * Gathers all term ids in the given tree and returns them as an array.
 * @return 
 * @param object $tree
 */
function tree_to_tids($tree) {
  $data = array();
  
  foreach ($tree as $term) {
    $data[] = $term->tid;
    
    if (!empty($term->children)) {
      $data = array_merge($data, tree_to_tids($term->children));
    }
  }

  return $data; 
}

/**
 * Exports specified vocabulary.
 * @return 
 * @param object $tree
 * @param object $depth[optional]
 */
function export_taxonomy_tree($tree, $depth = 0) {
  $output = '';
  
  foreach ($tree as $term) {
    for ($i = 0; $i < $depth; $i++) {
      $output .= '-'; 
    }
      
    $output .= $term->name . "\n";
    
    if (!empty($term->children)) {
      $output .= export_taxonomy_tree($term->children, ++$depth);
      $depth--;
    }
  }
  
  return $output; 
}

/**
 * Searches the specified vocabulary for a term with a matching name.
 * @return 
 * @param object $name
 * @param object $vid[optional]
 */
function get_tid_by_name($name, $vid = NULL) {
  $name = preg_replace('/[^a-zA-Z0-9\s]/', '', $name);
  $name = strtolower($name);
  $name = str_replace('  ', ' ', $name);
  $name = str_replace(' ', '%', $name);

  $query = "  SELECT  tid
              FROM    {term_data}
              WHERE   LCASE(name) LIKE '%%%s%'
           ";
       
  if (!empty($vid)) {
    $query .= " AND vid = %d ";
  }
  
  $query .= " LIMIT 1";

  $tid = db_result(db_query($query, $name, $vid));
  
  return $tid;
}

/**
 * Returns an array of nodes which are assigned to the given term ids.
 * @return 
 * @param object $tids
 * @param object $join
 * @param object $where
 */
function get_nodes_in_tids($tids, $join = '', $where = '') {   
  $nodes = array();
              
  if (!empty($tids)) {
    $query =  " SELECT      tn.nid AS nid
                FROM        {term_node} tn
                INNER JOIN  {term_data} td
                  ON        tn.tid = td.tid
                " . $join . "
                WHERE       tn.tid IN (%s)
                " . $where . "
                ORDER BY    td.weight ASC               
              ";
          
    $result = db_query($query, implode(',', $tids));
    
    while ($row = db_fetch_object($result)) {
      $nodes[$row->nid] = node_load($row->nid);
    }
  }
  
  return $nodes;
}
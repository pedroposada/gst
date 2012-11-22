<?php

/**
 * This function reads the selected tab-delimited file and builds Classified nodes for each record in the file. 
 *
 * @param $file
 *     The file to import.
 *
 * @return string
 *     
 */
function classified_form_upload_do($file = NULL) {
  // Read import file into an array
  $classaddumpfile = basename($file->destination, ".txt");
  $pattern = '/^www([0-9]){4,6}.TXT/';
  
  if (preg_match($pattern, $file->filename)) {
    $classads = file(file_directory_temp() . "/" . $classaddumpfile);  
    $vocabularies = taxonomy_get_vocabularies();
    $vid = 0;
    $taxonomy = array();
    $parent_taxonomy = array();
    
    foreach ($vocabularies as $key => $value) {
      if($value->machine_name == 'classifieds') {
        $vid = $value->vid;
      }
    }
    
    $taxonomy = taxonomy_get_tree($vid);
    $parent_taxonomy = taxonomy_get_tree($vid, 0, 1);
  
    if ($vid == 0) {
      // If no tids are found for Classifieds the function returns an error value of 0
      watchdog('classadimport', t("Classified Vocabulary not found"), $severity = WATCHDOG_WARNING, $link = NULL);
      $output = t("Classified ads could not be imported as no Drupal vocabulary for Classifieds could be found");
    }
    else {
      // Set number of days that Classified ads remain active
      $days_to_expire = 7;
      // Set the hour of day that Classified ads go live
      $activation_hour = 3;
      $output = '';
      $ads_not_imported = '';
      $house_ads = 0;
      $duplicate_entry = 0;
      $total_ads_not_imported = 0;
      $new_tax = 0;
      $rows = array();
      $new_tax_rows = array();
      $duplicate_rows = array();
      
      // Build nodes   
      for ($i = 0; $i < (count($classads)); $i++) {
        $is_tax = TRUE;
        $node = '';
        $node->type = "content_classified_ad";
        $classad = explode("\t", $classads[$i]);
        // Consolidate Real Estate sections per Classified Dept request    
        $classad[8] = preg_replace('/(Real Estate)(.*)/' , '${1} Directory', $classad[8]); 
        $node->title = $classad[8] . " - " . $classad[9] . " (Ad: " . $classad[1] . ")";
        $node->field_customer_number['und'][0]['value'] = $classad[0]; 
        $node->field_order_number['und'][0]['value'] = $classad[1];
        $node->field_subtype['und'][0]['value'] = $classad[5];
        // Change Gay Adult Services classification name to Adult Services - Gay 
        // and consolidate Female Escorts and Massage sections per Classified Dept request
        $classad[8] = str_replace('Gay Adult Services', 'Adult Services - Gay', $classad[8]);
        // Change Home & Garden Services to Home/Garden Services
        $classad[8] = str_replace('Home & Garden Services', 'Home/Garden Services', $classad[8]);
        $node->field_classsection['und'][0]['value'] = $classad[8];
        //$classad[9] = preg_replace('/(Female Escorts)(.*)/' , '${1}', $classad[9]);
        //$classad[9] = preg_replace('/(Massage)(.*)/' , '${1}', $classad[9]);
        $classad[9] = preg_replace('/(.*)(-LINE)/' , '${1}', $classad[9]);
        $classad[9] = preg_replace('/(.*)(-DISPLAY)/' , '${1}', $classad[9]);
        $classad[9] = preg_replace('/(.*)( - Special)/' , '${1}', $classad[9]);  
  
        $parent_term_tid = 0;
        $child_term_tid = 0;
        $child_term_array = array(); //in the case where there are more than one taxonomy children terms which are the same (like Services), we must check that the parent is the correct one, if not fix.
        $taxonomy = taxonomy_get_tree($vid);
        
        foreach($taxonomy as $key => $term) {
          if ($term->name == $classad[8] && $term->depth == 0) {
            $parent_term_tid = $term->tid;
          }
        if ($term->name == $classad[9]) {
          array_push($child_term_array, $term->tid);
          $child_term_tid = $term->tid;
        }
      } // END Build Nodes
  
      if ($parent_term_tid != 0) {
        if (count($child_term_array) > 1) {//if multiple children, take the correct one
          $parent_term_array = taxonomy_get_tree($vid, $parent_term_tid);
          
          foreach($child_term_array as $child_key => $child_term) {
            foreach($parent_term_array as $parent_key => $parent_term) {
              if ($parent_term->tid == $child_term) {
                $child_term_tid = $parent_term->tid;
              }
            }
          }
        }
        elseif (count($child_term_array) == 1) {//if one child, test to see if the parent corresponds, if not we are talking about a new taxonomy term which then needs to be created
          $parent_term_array = taxonomy_get_tree($vid, $parent_term_tid);
          $child_term_exists = FALSE;
          
          foreach($parent_term_array as $key => $term) {
            if($term->tid == $child_term_tid) {
              $child_term_exists = TRUE;
            }
          }
          
          if (!$child_term_exists) {
            $child_term_tid = 0;
          }
        }  
      }
      
      if ($parent_term_tid == 0) {
        $total_ads_not_imported += 1;$is_tax = FALSE;
        $rows[] = array($classad[1], $classad[8]);
      }
      else {
        if ($child_term_tid == 0) {
          $term = new stdClass();
          $term->vid = $vid;
          $term->name = $classad[9];
          $term->parent = $parent_term_tid;
          taxonomy_term_save($term);//Save new taxonomy term
  
          $node->field_class_tax['und'][0]['tid'] = $parent_term_tid;
          $node->field_class_tax['und'][1]['tid'] = $term->tid;
          $new_tax_rows[] = array($term->tid, $classad[9], $classad[8]);
          $new_tax++;
        }
        else {
          $node->field_class_tax['und'][0]['tid'] = $parent_term_tid;
          $node->field_class_tax['und'][1]['tid'] = $child_term_tid;
        }
      }
      
      //set language to en
      $node->language = "und";
      $node->field_classification['und'][0]['value'] = str_replace('/','-',$classad[9]);
      $node->field_keywords['und'][0]['value'] = _gs_classifieds_strip_characters($classad[10]);
      $node->body['und'][0]['value'] = _gsclassifieds_add_image_tags(_gsclassifieds_clean_body($classad[11]));
      // Clean body of classified ad
      //$node->body['und'][0]['value'] = "<hr class=\"classifieds-body-hr\">" . _gsclassifieds_add_image_tags(_gsclassifieds_clean_body($classad[11]))  . "</p>";
      //$node->body['und'][0]['summary'] = "<hr class=\"classifieds-summary-hr\">" . _gsclassifieds_remove_images(_gsclassifieds_clean_body($classad[11])) . "</p>";
      $node->body['und'][0]['format'] = 'full_html';//or title_html
      // Create Unix time value for Activation Date and add 7 days for Expiry Date
      $timestamp_array = _gsclassifieds_date_conversion($classad[12], $activation_hour, $days_to_expire);
      $node->field_activation_date['und'] = array('0' => array('value' => $timestamp_array['activation']));
      $node->field_activation_date['und'][0]['timezone'] = 'none';
      $node->field_expiry_date['und'] = array('0' => array('value' => $timestamp_array['expiry']));
      $node->field_expiry_date['und'][0]['timezone'] = 'none';
  
      if (isset($classad[13])) {
        // Sort Codes are a new features so we'll put this condition in to make sure we don't break old uploads.
        $node->field_sortcode['und'][0]['value'] = (integer) trim($classad[13]);
      } 
  
      // create ad if it's not a house ad
      if ($classad[0] != 10031121) {
      
        // check and see if ad already exists, in that case do not create a duplicate
        $sql_duplicate_select = "SELECT * FROM {field_data_field_order_number} i 
                                 LEFT JOIN {field_data_field_sortcode} a ON i.entity_id = a.entity_id 
                                 WHERE i.field_order_number_value = " . $node->field_order_number['und'][0]['value'] . " 
                                 AND a.field_sortcode_value = " . $node->field_sortcode['und'][0]['value'];
  
        if ((bool) db_query_range($sql_duplicate_select, 0, 1)->fetchField()) {
          if ($is_tax) {//If the ad does not have a correct taxonomy do not increment the duplicate entry variable
            $duplicate_entry++;
            $duplicate_rows[] = array($classad[1], $classad[8], (integer) trim($classad[13]));
            $duplicate_result = Database::getConnection()->query($sql_duplicate_select . " LIMIT 0, 1")->fetch();        
            $duplicate_node = node_load($duplicate_result->entity_id);
            $node->nid = $duplicate_node->nid;
            $node->vid = $duplicate_node->vid;
            $node->uid = $duplicate_node->uid;
            $node->is_new = FALSE;
            $node = node_submit($node);
            node_object_prepare($node);
            node_save($node);
          }
        }
        else {
          $node = node_submit($node);
          node_object_prepare($node);
          node_save($node);
        }
      }
      else {
        $house_ads++;
      }
    }
  
    if ($i) {
      $i = $i - $total_ads_not_imported - $duplicate_entry;
      $output .= "<p>Imported " . ($i - $house_ads) . " classified ads.</p>";
      $output .= "<p>Ignored ". $house_ads . " house ads.</p>";
      $output .= "<p>Updated ". $duplicate_entry . " classified ads.</p>";
  
      if ($total_ads_not_imported > 0) {
        $output .= "<p>The following " . $total_ads_not_imported . " classified ads were NOT IMPORTED because there was no matching Drupal taxonomy:</p>";
        $header = array("Order number", "Class Section");
        $output .= theme('table', array('header' => $header, 'rows'=> $rows));
      }   
  
      if ($duplicate_entry > 0) {
        $output .= "<p>The following " . $duplicate_entry . " classified ads were UPDATED as they are already in the system:</p>";
        $header = array("Order number", "Class Section", "Sort Code");
        $output .= theme('table', array('header' => $header, 'rows'=> $duplicate_rows));
      }
  
      if ($new_tax > 0) {
        $output .= "<p>The following " . $new_tax . " taxonomy terms did not exists on the site and were SUCCESSFULLY ADDED</p>";
        $header = array("Taxonomy ID", "Taxonomy name", "Taxonomy parent");
        $output .= theme('table', array('header' => $header, 'rows'=> $new_tax_rows));
      }
    }
    else {
      $output .= "No ads were imported.";
    }
  
    $output .= "<p>";
  
    $query = "DELETE FROM {cache} WHERE cid LIKE 'classifieds_%'";
  
    $result = db_query($query);
  
    // Display imported records
    return $output;
    } 
  }
  else {  
    $output .= '<div class="content"><h1>File Name Error</h1><p>The name of the file that you are trying to import (' . $classaddumpfile . ') does not match the name of files output from the APT system and so cannot be imported.</p><p>File names typically have the letters "www" followed by a date and ending with a .TXT extension, for example, www122806.TXT</p><p>Please check the file you are trying to import.</p></div>';
  }  
  
  // Display error if ads could not be imported 
  return $output; 
}
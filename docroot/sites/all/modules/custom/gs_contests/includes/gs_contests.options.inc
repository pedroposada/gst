<?php

/**
 * Options list containing Canadian provinces and territories.
 */
function gs_contests_options_canada($component, $flat, $filter, $arguments) {
  return array(
    'AB' => t('Alberta'),
    'BC' => t('British Columbia'),
    'MB' => t('Manitoba'),
    'NB' => t('New Brunswick'),
    'NL' => t('Newfoundland and Labrador'),
    'NT' => t('Northwest Territories'),
    'NS' => t('Nova Scotia'),
    'NU' => t('Nunavut'),
    'ON' => t('Ontario'),
    'PE' => t('Prince Edward Island'),
    'QC' => t('Quebec'),
    'SK' => t('Saskatchewan'),
    'YT' => t('Yukon'),
  );
}

/**
 * Options list containing major cities in the GVRD, Coast and Mountain regions.
 */
function gs_contests_options_bc_cities($component, $flat, $filter, $arguments) {
  return array(
    'Abbotsford' => t('Abbotsford'),
		'Anmore' => t('Anmore'),
		'Belcarra' => t('Belcarra'),
    'Bowen Island' => t('Bowen Island'),
    'Burnaby' => t('Burnaby'),
    'Chilliwack' => t('Chilliwack'),
		'Coquitlam' => t('Coquitlam'),
    'Delta' => t('Delta'),
    'Fort Langley' => t('Fort Langley'),
    'Gibsons' => t('Gibsons'),
    'Harrison Hot Springs' => t('Harrison Hot Springs'),
    'Hope' => t('Hope'),
		'Kent' => t('Kent'),
    'Langley' => t('Langley'),
    'Lytton' => t('Lytton'),
    'Maple Ridge' => t('Maple Ridge'),
    'Mission' => t('Mission'),
    'New Westminster' => t('New Westminster'),
    'North Vancouver' => t('North Vancouver'),
    'Pemberton' => t('Pemberton'),
    'Pitt Meadows' => t('Pitt Meadows'),
		'Port Coquitlam' => t('Port Coquitlam'),
    'Port Moody' => t('Port Moody'),
    'Powell River' => t('Powell River'),
    'Richmond' => t('Richmond'),
    'Sechelt' => t('Sechelt'),
    'Squamish' => t('Squamish'),
    'Surrey' => t('Surrey'),
    'Vancouver' => t('Vancouver'),
    'West Vancouver' => t('West Vancouver'),
    'Whistler' => t('Whistler'),
    'White Rock' => t('White Rock'),
  );
}
<?php
// $Id: ds.info.inc,v 1.1.2.1 2009/10/24 13:32:28 swentel Exp $

/**
 * @file
 * Info about a module.
 */

function ds_info($module) {
  $info = '';

  // Gather information.
  $function = $module .'_ds_api';
  $api_data = $function();

  // General info (if any).
  if (isset($api_data['help'])) {
    $fieldset['#title'] = t('General info');
    $fieldset['#value'] = $api_data['help'];
    $info .= theme('fieldset', $fieldset);
  }

  // API fields.

  if (!empty($info)) {
    return $info;
  }
  else {
    return t('No general info found.');
  }
}
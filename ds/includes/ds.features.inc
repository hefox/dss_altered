<?php
// $Id: ds.features.inc,v 1.1.2.3 2009/11/30 08:44:41 swentel Exp $

/**
 * @file
 * Features support.
 */

/**
 * Return API information for features.
 */
function _ds_features_api() {
  return array(
    'ds' => array(
      'name' => t('Display suite'),
      'default_hook' => 'ds_default_settings',
      'default_file' => FEATURES_DEFAULTS_INCLUDED,
      'features_source' => TRUE,
      'file' => drupal_get_path('module', 'ds') .'/includes/ds.features.inc',
    ),
  );
}

/**
 * Implementation of hook_features_export_options().
 */
function ds_features_export_options() {
  return _ds_features_get_types();
}

/**
 * Get types for features.
 */
function _ds_features_get_types() {
  $types = array();
  foreach (module_implements('ds_api') as $module) {
    $function = $module .'_ds_api';
    $info = $function();
    foreach ($info['types']() as $tkey => $type) {
      $types[$module .'-'. $tkey] = $type->name;
    }
  }
  return $types;
}

/**
 * Implementation of hook_features_export.
 */
function ds_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  // Display suite is always dependant.
  $export['dependencies']['ds'] = 'ds';

  foreach ($data as $type) {
    list($module, $dstype) = explode('-', $type);
    $export['dependencies'][$module] = $module;
    $export['features']['ds'][$module] = $module .'-'. $dstype;
  }

  return $pipe;
}

/**
 * Implementation of hook_features_export_render().
 */
function ds_features_export_render($module = 'foo', $data) {
  $export = array();
  foreach ($data as $type) {
    list($module, $dstype) = explode('-', $type);
    $settings = variable_get($module .'_display_settings_'. $dstype, array());
    $export[$module][$dstype] = $settings;
  }
  $export_value = '$data = '. var_export($export, TRUE) .";";
  return array('ds_default_settings' => $export_value);
}

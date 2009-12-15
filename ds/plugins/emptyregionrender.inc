<?php
// $Id: emptyregionrender.inc,v 1.1.2.3 2009/10/24 13:32:28 swentel Exp $

/**
 * @file
 * Plugin to render an empty region.
 */

/**
 * Implementation of ds_pluginname_form().
 */
function ds_emptyregionrender_form(&$form, $display_settings) {

  $form['emptyregionrender'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Empty region render'),
    '#description' => t('Toggle regions you want to render when there are no fields in it or when field values are empty'),
  );

  $build_mode = $form['#build_mode'];
  $regions = ds_regions();
  foreach ($regions as $region => $title) {
    if ($region != 'disabled') {
      $form['emptyregionrender']['region-render-'. $region] = array(
        '#title' => $title,
        '#type' => 'checkbox',
        '#default_value' => ds_default_value($display_settings, $build_mode, 'emptyregionrender', 'render', $region, ''),
      );
    }
  }
}

/**
 * Implementation of ds_pluginname_submit().
 */
function ds_emptyregionrender_submit($form, $form_state, &$display_settings) {
  $regions = ds_regions();
  $regions_save = array();
  $build_mode = $form['#build_mode'];
  foreach ($regions as $region => $title) {
    $regions_save['render'][$region] = $form_state['values']['emptyregionrender']['region-render-'. $region];
  }
  $display_settings[$build_mode]['emptyregionrender'] = $regions_save;
}

/**
 * Implementation of ds_pluginname_process().
 */
function ds_emptyregionrender_process(&$vars, &$display, $display_settings, $object_type) {
  foreach ($display->all_regions as $region_name => $region_title) {
    $render = ds_default_value($display_settings, $vars[$object_type]->build_mode, 'emptyregionrender', 'render', $region_name, FALSE);
    if ($render == TRUE && !isset($display->themed_regions[$region_name])) {
      $display->themed_regions[$region_name] = array('content' => '&nbsp;');
      if ($region_name == 'left' || $region_name == 'right') {
        $display->region_classes[$region_name] = $region_name;
      }
    }
  }
}

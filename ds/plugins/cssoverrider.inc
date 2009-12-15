<?php
// $Id: cssoverrider.inc,v 1.1.2.2 2009/11/11 14:49:20 swentel Exp $

/**
 * @file
 * Plugin to override css inline.
 */

/**
 * Implementation of ds_pluginname_form().
 */
function ds_cssoverrider_form(&$form, $display_settings) {

  $form['cssoverrider'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('CSS overrider'),
    '#description' => t('Either add extra classes or override CSS inline per region.'),
  );

  $build_mode = $form['#build_mode'];
  $regions = ds_regions();
  foreach ($regions as $region => $title) {
    if ($region != 'disabled') {
      $form['cssoverrider']['region-class-'. $region] = array(
        '#title' => $title .' class',
        '#type' => 'textfield',
        '#description' => t('Enter extra classes, seperated by comma.'),
        '#default_value' => ds_default_value($display_settings, $build_mode, 'cssoverrider', 'class', $region, ''),
      );
      $form['cssoverrider']['region-inline-'. $region] = array(
        '#title' => $title .' inline css',
        '#type' => 'textfield',
        '#description' => t('Enter inline css. Note, you only need to enter the css rules, the plugin will add style="" for you.'),
        '#default_value' => ds_default_value($display_settings, $build_mode, 'cssoverrider', 'inline', $region, ''),
      );
    }
  }
}

/**
 * Implementation of ds_pluginname_submit().
 */
function ds_cssoverrider_submit($form, $form_state, &$display_settings) {
  $regions = ds_regions();
  $css_save = array();
  $build_mode = $form['#build_mode'];
  foreach ($regions as $region => $title) {
    $css_save['class'][$region] = $form_state['values']['cssoverrider']['region-class-'. $region];
    $css_save['inline'][$region] = $form_state['values']['cssoverrider']['region-inline-'. $region];
  }
  $display_settings[$build_mode]['cssoverrider'] = $css_save;
}

/**
 * Implementation of ds_pluginname_process().
 */
function ds_cssoverrider_process(&$vars, &$display, $display_settings, $object_type) {
  foreach ($display->themed_regions as $region_name => $region_data) {
    $classes = ds_default_value($display_settings, $vars[$object_type]->build_mode, 'cssoverrider', 'class', $region_name, '');
    $inline = ds_default_value($display_settings, $vars[$object_type]->build_mode, 'cssoverrider', 'inline', $region_name, '');
    if (!empty($classes)) {
      $display->themed_regions[$region_name]['extra_class'] = ' '. $classes;
    }
    if (!empty($inline)) {
      $display->themed_regions[$region_name]['inline_css'] = 'style="'. $inline .'"';
    }
  }
}

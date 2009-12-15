<?php
// $Id: ds.plugins.inc,v 1.1.2.1 2009/10/24 13:32:28 swentel Exp $

/**
 * @file
 * Administration functions for DS plugins.
 */

/**
 * Menu callback. Registers plugins for a module.
 */
function ds_plugins($form_state, $module) {
  $form = array();
  $form['#module'] = $module;

  $plugins = ds_plugins_retrieve($module);
  $plugin_settings = variable_get($module .'_plugin_settings', array());
  if (is_array($plugins)) {
    $form['#plugins'] = $plugins;
    foreach ($plugins as $key => $plugin) {
      $form['plugin_'. $plugin['key']] = array(
        '#type' => 'checkbox',
        '#default_value' => $plugin_settings[$plugin['key']]['status'],
      );
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Save the plugin settings.
 */
function ds_plugins_submit($form, &$form_state) {
  $plugin_settings = array();
  foreach ($form['#plugins'] as $key => $plugin) {
    if ($form_state['values']['plugin_'. $plugin['key']] == '1') {
      $plugin_settings[$plugin['key']] = $plugin;
      $plugin_settings[$plugin['key']]['status'] = '1';
    }
  }
  variable_set($form['#module'] .'_plugin_settings', $plugin_settings);
}

/**
 * Theme the form.
 */
function theme_ds_plugins($form) {
  $output = '';

  $header = array(
    t('Status'),
    t('Info'),
  );

  $rows = array();
  foreach ($form['#plugins'] as $key => $plugin) {
    $row = array();
    $row[] = array('data' => drupal_render($form['plugin_'. $plugin['key']]));
    $row[] = array('data' => '<strong>'. $plugin['title'] .'</strong>: '. $plugin['description']);
    $rows[] = $row;
  }

  $output .= theme('table', $header, $rows);
  $output .= drupal_render($form);

  return $output;
}

/**
 * Retrieve all available plugins.
 */
function ds_plugins_retrieve($module) {
  $plugins = module_invoke_all('ds_plugins');
  foreach ($plugins as $plugin => $data) {
    if ($data['target'] == 'all' || (is_array($data['target']) && isset($data['target'][$module]))) {
      $data['key'] = $plugin;
      $data['form_callback'] = 'ds_'. $plugin .'_form';
      $data['submit_callback'] = 'ds_'. $plugin .'_submit';
      $data['process_callback'] = 'ds_'. $plugin .'_process';
      if (isset($data['file']) && isset($data['path']))
      $data['filename'] = $data['path'] .'/'. $data['file'];
      $plugins[$plugin] = $data;
    }
  }
  return $plugins;
}


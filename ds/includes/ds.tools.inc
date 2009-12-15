<?php
// $Id: ds.tools.inc,v 1.1.2.1 2009/11/28 22:39:15 swentel Exp $

/**
 * @file
 * Tools for Display suite like export & import.
 */

/**
 * Import functionality.
 */
function ds_import($form_state) {
  $form = array();

  $form['import'] = array(
    '#title' => t('Import data'),
    '#description' => t('Paste data to import field settings. Do not include the function declaration from the export. <strong>Warning: existing data will be overwritten!</strong>'),
    '#type' => 'textarea',
    '#cols' => 60,
    '#default_value' => '',
    '#required' => TRUE,
    '#rows' => 10,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );

  return $form;
}

/**
 * Import submit function.
 */
function ds_import_submit($form_id, &$form_state) {
  $error = FALSE;

  ob_start();
  eval($form_state['values']['import']);
  ob_end_clean();

  // Validate the data.
  if (!is_array($data)) {
    $error = TRUE;
    drupal_set_message(t('Data string is not an array.'), 'error');
  }

  // All ok, let's import.
  if ($error == FALSE) {
    $data = $data;
    foreach ($data as $module => $value) {
      foreach ($value as $type => $settings) {
        dsm(t('Saved display settings for @module and @type', array('@module' => $module, '@type' => $type)));
        variable_set($module .'_display_settings_'. $type, $settings);
      }
    }
  }
}

/**
 * Export functionality.
 */
function ds_export(&$form_state) {
  $form = array();
  $options = array();
  $step = isset($form_state['storage']['step']) ? $form_state['storage']['step'] + 1 : 1;

  switch ($step) {

    // Show selection form.
    case 1:
      foreach (module_implements('ds_api') as $module) {
        $function = $module .'_ds_api';
        $info = $function();
        $module = $info['module'];
        foreach ($info['types']() as $tkey => $type) {
          $options[$module .'-'. $tkey] = $type->name;
        }
      }

      if (!empty($options)) {
        $form['#prefix'] = t('Select one or more types to export.');
        $form['types'] = array(
          '#title' => 'Types',
          '#type' => 'checkboxes',
          '#options' => $options,
          '#required' => TRUE,
        );
      }
      else {
        $form['info'] = array(
          '#type' => 'item',
          '#value' => t('No object types found to export.'),
        );
      }
      break;

    // Show export.
    case 2:
      $export = array();
      foreach ($form_state['values']['types'] as $otype => $value) {
        if ($otype === $value) {
          list($module, $type) = explode('-', $value);
          $settings = variable_get($module .'_display_settings_'. $type, array());
          if (!empty($settings)) {
            $export[$module][$type] = $settings;
          }
        }
      }

      if (!empty($export)) {
        $export_value = var_export($export, TRUE);
        // @todo need to come up with a file convention so we can use default imports
        // without having to use the import form all the time. Will be integrated with
        // features also (for inspiration, let's look at views and content).
        $form['export'] = array(
          '#title' => t('@module export data', array('@module' => drupal_strtoupper($module))),
          //'#description' => t('You can paste this data into @path. If you only want to store this data to import later, omit the function declaration.', array('@path' => drupal_get_path('module', $module) .'/'. $module .'.ds.default.inc')),
          //'#value' => "<?php\n/**\n * @file\n * Display suite export data for $module\n */\n\nfunction {$module}_ds_default() {\n\$data = ". $export_value .";\nreturn \$data;\n}",
          '#value' => "\$data = ". $export_value .";",
          '#type' => 'textarea',
          '#cols' => 60,
          '#rows' => 15,
        );
      }
      else {
        $form['info'] = array(
          '#type' => 'item',
          '#value' => t('No settings found to export.'),
        );
      }

      break;
  }

  if ($step == 1 && !empty($options)) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Export'),
    );
  }

  $form['step'] = array(
    '#type' => 'value',
    '#value' => $step,
  );

  return $form;
}

/**
 * Export submit function.
 */
function ds_export_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $form_state['storage']['step'] = $form_state['values']['step'];
}

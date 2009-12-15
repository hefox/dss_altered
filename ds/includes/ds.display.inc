<?php
// $Id: ds.display.inc,v 1.1.2.12 2009/11/28 22:39:15 swentel Exp $

/**
 * @file
 * Display overview form.
 */

/**
 * General page.
 */
function ds_layout_general() {

  $path = db_result(db_query("SELECT path FROM {menu_router} WHERE path LIKE 'admin/ds/layout/%%' AND number_parts = 4 ORDER BY weight DESC"));
  if ($path != FALSE) {
    drupal_goto($path);
  }

  return t('You have no object types.');
}

/**
 * Menu callback; presents a listing of fields display settings for an object type.
 *
 * Form includes form widgets to select which fields appear for teaser, full node
 * etc and how the field labels should be rendered.
 *
 * This function is based on content_display_overview_form from
 * content.admin.inc of cck, but altered to have weights and regions.
 */
function ds_display_overview_form(&$form_state, $type, $build_mode = 'full', $module = 'nd') {

  // Gather information.
  $function = $module .'_ds_api';
  $api_data = $function();
  $display_settings = variable_get($module .'_display_settings_'. $type, array());

  $form = array(
    '#tree' => TRUE,
    '#module' => $module,
    '#type_name' => $type,
    '#fields' => array(),
    '#build_mode' => $build_mode,
    '#plugins' => FALSE,
    '#extra' => array(),
  );

  // Extra info.
  if (isset($api_data['extra'])) {
    $types = $api_data['types']();
    $type_info = $types[$type];
    foreach ($api_data['extra'] as $key) {
      $form['#extra'][$key] = $type_info->{$key};
    }
  }

  // Exclude build mode.
  $exclude_build_modes = variable_get($module .'_buildmodes_exclude', array());
  $excluded = ((isset($exclude_build_modes[$type][$build_mode])
    && $exclude_build_modes[$type][$build_mode] == TRUE) ||
    variable_get($module .'_type_'. $type, FALSE) == TRUE) ? TRUE : FALSE;
  $form['exclude_build_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Exclude this build mode from rendering.'),
    '#default_value' => $excluded,
    '#attributes' => array('onClick' => 'toggleFieldOverviewForm()'),
  );

  $global_exclude = variable_get($module .'_type_'. $type, FALSE);
  $form['exclude_build_mode']['#disabled'] = $global_exclude;
  if ($global_exclude == TRUE) {
    $description = t('The checkbox has been disabled because the complete type has been excluded from rendering.') .'<br />';
    $form['exclude_build_mode']['#description'] = $description .' '. $form['exclude_build_mode']['#description'];
  }

  // If global exclude is true, no point in rendering.
  if ($global_exclude == FALSE) {
    // Add the fields.
    ds_fields_display_form($form, $display_settings);

    // Plugins.
    $plugins = TRUE;
    if (isset($api_data['plugins_exclude']) && in_array($build_mode, $api_data['plugins_exclude'])) {
      $plugins = FALSE;
    }
    if ($plugins)
    ds_plugins_display_form($form, $display_settings);

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }

  return $form;
}

/**
 * Add fields to display overview form.
 */
function ds_fields_display_form(&$form, $display_settings) {

  $module = $form['#module'];
  $build_mode = $form['#build_mode'];
  $type = $form['#type_name'];
  $extra = $form['#extra'];

  $fields = ds_get_fields($module, $type, $build_mode, $extra, TRUE);
  if (!empty($fields)) {
    foreach ($fields as $field => $value) {
      $form['#fields'][] = $field;
      $form[$field] = (!empty($form[$form['#fields'][0]])) ? $form[$form['#fields'][0]] : ds_field_default_form_properties($build_mode);
      $form[$field]['human_name']['#value'] = $value['title'];
      $form[$field]['weight']['#default_value'] = ds_default_value($display_settings, $build_mode, 'fields', $field, 'weight', DS_DEFAULT_WEIGHT);
      $form[$field]['ds_weight']['#default_value'] = ds_default_value($display_settings, $build_mode, 'fields', $field, 'weight', DS_DEFAULT_WEIGHT);
      $form[$field][$build_mode]['label']['format']['#default_value'] = isset($value['display_settings'][$build_mode]['label']['format']) ? $value['display_settings'][$build_mode]['label']['format'] : ds_default_value($display_settings, $build_mode, 'fields', $field, 'labelformat', DS_DEFAULT_LABEL_FORMAT);
      $form[$field][$build_mode]['format']['#options'] = isset($value['formatters']) ? $value['formatters'] : array();
      $format = ds_default_value($display_settings, $build_mode, 'fields', $field, 'format', '');
      if (empty($format) && isset($value['formatters'])) {
        $format = key($value['formatters']);
      }
      $form[$field][$build_mode]['format']['#default_value'] = $format;
      $form[$field][$build_mode]['format']['#access'] = (count($form[$field][$build_mode]['format']['#options']) == 1 || count($form[$field][$build_mode]['format']['#options']) == 0) ? FALSE : TRUE;
      $form[$field][$build_mode]['region']['#default_value'] = ds_default_value($display_settings, $build_mode, 'fields', $field, 'region', DS_DEFAULT_REGION);
    }
  }
}

/**
 * Add plugins to display overview form.
 *
 * @param array $form The display form.
 * @param array $display_settings Current display settings.
 */
function ds_plugins_display_form(&$form, $display_settings) {
  $module = $form['#module'];
  $plugins = variable_get($module .'_plugin_settings', array());
  if (!empty($plugins)) {
    $form['#plugins'] = TRUE;
    $form['#plugin_keys'] = array();
  }

  foreach ($plugins as $key => $plugin) {
    if (isset($plugin['filename'])) {
      require_once($plugin['filename']);
    }
    $function = $plugin['form_callback'];
    $plugin_form = $function($form, $display_settings);
    $form['#plugin_keys'][] = $key;
  }
}

/**
 * Save fields & plugins for a build mode.
 */
function ds_display_overview_form_submit($form, &$form_state) {

  $module = $form['#module'];
  $build_mode = $form['#build_mode'];
  $type = $form['#type_name'];
  $extra = $form['#extra'];

  // Exclude build mode checkbox.
  $exclude_build_mode = $form_state['values']['exclude_build_mode'];
  $exclude_build_modes = variable_get($module .'_buildmodes_exclude', array());
  $exclude_build_modes[$type][$build_mode] = $exclude_build_mode;
  variable_set($module .'_buildmodes_exclude', $exclude_build_modes);

  $display_settings = variable_get($module .'_display_settings_'. $type, array());
  unset($display_settings[$build_mode]);

  // Fields.
  $fields = ds_get_fields($module, $type, $build_mode, $extra);
  if (!empty($fields)) {
    foreach ($fields as $key => $field) {
      $display_settings[$build_mode]['fields'][$key]['weight'] = $form_state['values'][$key]['ds_weight'];
      $display_settings[$build_mode]['fields'][$key]['format'] = $form_state['values'][$key][$build_mode]['format'];
      $display_settings[$build_mode]['fields'][$key]['region'] = $form_state['values'][$key][$build_mode]['region'];
      $display_settings[$build_mode]['fields'][$key]['labelformat'] = $form_state['values'][$key][$build_mode]['label']['format'];

      // Ditch this field, unless it has its storage in another module (ie CCK)
      if (!isset($field['storage']))
      unset($form_state['values'][$key]);
    }
  }

  // Plugins
  if ($form['#plugins'] == TRUE)
  ds_plugins_display_submit($form, $form_state, $display_settings);

  // Save all these settings.
  variable_set($module .'_display_settings_'. $type, $display_settings);

  drupal_set_message(t('Your settings have been saved.'));
}

/**
 * Save plugins settings.
 *
 * @param array $form The display form.
 * @param array $form_state The submitted values of the form.
 * @param array $display_settings Current display settings being saved.
 */
function ds_plugins_display_submit($form, $form_state, &$display_settings) {
  $module = $form['#module'];

  $plugins = variable_get($module .'_plugin_settings', array());

  foreach ($plugins as $key => $plugin) {
    if (isset($plugin['filename'])) {
      require_once($plugin['filename']);
    }
    $function = $plugin['submit_callback'];
    $function($form, $form_state, $display_settings);
  }
}

/**
 * Return array of available label options.
 */
function ds_label_options() {
  return array(
    'above' => t('Above'),
    'inline' => t('Inline'),
    'hidden' => t('Hidden'),
  );
}

/**
 * Function to load default form properties for a field in a context
 */
function ds_field_default_form_properties($build_mode) {

  $field = array(
    'label' => array(
      'format' => array(
        '#type' => 'select',
        '#options' => ds_label_options(),
        '#default_value' => DS_DEFAULT_LABEL_FORMAT,
      ),
    ),
    'format' => array(
      '#type' => 'select',
      '#options' => array(),
      '#default_value' => DS_DEFAULT_FORMAT,
    ),
    'region' => array(
      '#type' => 'select',
      '#options' => ds_regions(variable_get('ds_build_mode_'. $build_mode, 'all')),
      '#default_value' => DS_DEFAULT_REGION,
      '#attributes' => array(
        'class' => 'field-region-select field-region-'. DS_DEFAULT_REGION,
      ),
    ),
  );

  // If build mode is not empty, this means we're looking at
  // a content type which has no cck fields defined. Add
  // ds_weight and build mode key to the default field array
  // so the draghandling & formatters start working.
  if (!empty($build_mode)) {
    $field['ds_weight'] = array(
      '#type' => 'weight',
      '#default_value' => -19,
      '#delta' => 20,
    );
    $field[$build_mode] = array(
      'label' => array(
        'format' => array(
          '#type' => 'select',
          '#options' => ds_label_options(),
          '#default_value' => 'hidden',
        ),
      ),
      'format' => array(
        '#type' => 'select',
        '#options' => array(),
        '#default_value' => 'default',
      ),
      'region' => array(
        '#type' => 'select',
        '#options' => ds_regions(variable_get('ds_build_mode_'. $build_mode, 'all')),
        '#default_value' => '',
        '#attributes' => array(
          'class' => 'field-region-select field-region-disabled',
        ),
      ),
    );
  }

  return $field;
}

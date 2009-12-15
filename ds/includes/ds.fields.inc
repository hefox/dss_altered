<?php
// $Id: ds.fields.inc,v 1.1.2.5 2009/11/28 15:06:20 swentel Exp $

/**
 * @file
 * Manage fields.
 */

/**
 * Fields overview.
 */
function ds_fields($module) {
  $output = '';
  $action = arg(4);
  $field = arg(5);

  // Gather API information.
  $function = $module .'_ds_api';
  $api_data = $function();
  $object = $api_data['object'];

  // Extra info.
  $extra = array();

  // Get fields.
  $db_fields = variable_get($module .'_fields', array());
  $all_fields = ds_get_fields($module, NULL, NULL, $extra, TRUE);

  // Compare with db_fields.
  foreach ($all_fields as $key => $db_field) {
    if ($db_field['type'] == DS_FIELD_OVERRIDABLE && !isset($db_field[$key])) {
      $db_fields[$key] = array(
        'title' => $db_field['title'],
        'code' => $db_field['code'],
        'type' => $db_field['type'],
        'exclude' => isset($db_field['exclude']) ? $db_field['exclude'] : array(),
      );
    }
  }

  // Delete form.
  if (in_array($field, array_keys($db_fields)) && $action == 'delete') {
    $output .= drupal_get_form('ds_field_delete_form', $module, $field, $db_fields[$field]);
  }
  // Edit form.
  elseif (in_array($field, array_keys($db_fields)) && $action == 'edit') {
    $form_id = ($db_fields[$field]['type'] == DS_FIELD_BLOCK) ? 'ds_block_field_form' : 'ds_code_field_form';
    $output .= drupal_get_form($form_id, $module, $object, $field, $db_fields[$field]);
  }
  // Custom fields overview form
  else {
    if (empty($db_fields)) {
      $output .= '<p>'. t('You have not defined any custom fields.') .'</p>';
    }
    else {
      $header = array(
        t('Title'), t('Key'), t('Type'), t('Operations'),
      );

      $rows = array();
      foreach ($db_fields as $key => $value) {
        $row = array();
        $row[] = check_plain($value['title']);
        $row[] = $key;
        $type = ($value['type'] == DS_FIELD_OVERRIDABLE) ? t('Default') : (($value['type'] == DS_FIELD_OVERRIDDEN) ? t('Overridden') : t('Custom'));
        if ($value['type'] == DS_FIELD_BLOCK) $type = t('Block field');
        $row[] = $type;
        $operations = l(t('Edit'), 'admin/ds/'. $module .'/fields/edit/'. $key);
        if ($value['type'] != DS_FIELD_OVERRIDABLE) {
          $text = ($value['type'] == DS_FIELD_OVERRIDDEN) ? 'Reset' : 'Delete';
          $operations .= ' - '. l(t($text), 'admin/ds/'. $module .'/fields/delete/'. $key);
        }
        $row[] = $operations;
        $rows[] = $row;
      }

      $output .= theme('table', $header, $rows);
    }

    // New fields form.
    $output .= drupal_get_form('ds_code_field_form', $module, $object);
    $output .= drupal_get_form('ds_block_field_form', $module);
  }

  return $output;
}

/**
 * Code field form.
 *
 * @param string $key The key of the field.
 * @param array $field The field with title and code keys.
 */
function ds_code_field_form($form_state, $module, $object, $key = '', $field = array()) {
  $form = array();

  if (empty($field)) {
    $field = array('title' => '', 'code' => '', 'exclude' => array(), 'type' => DS_FIELD_CUSTOM);
  }

  $form['code_identity'] = array(
    '#type' => 'fieldset',
    '#title' => (empty($key)) ? t('Add new code field'): t('Update code field'),
    '#collapsible' => (empty($key)) ? TRUE : FALSE,
    '#collapsed' => (empty($key)) ? TRUE : FALSE,
  );
  $form['code_identity']['code_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Field key'),
    '#description' => t('The machine-readable name of this field.'),
    '#required' => TRUE,
  );
  if (!empty($key)) {
    $form['code_identity']['code_key']['#disabled'] = TRUE;
    $form['code_identity']['code_key']['#value'] = $key;
    $form['code_identity']['code_key']['#description'] = t('The machine-readable name of this field. Note: you can not edit this field.');
  }
  $form['code_identity']['code_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Field title'),
    '#description' => t('The title to use when rendering the output and on the display administration screen.'),
    '#required' => TRUE,
    '#default_value' => $field['title']
  );
  $ctypes = node_get_types('names');
  $form['code_identity']['code_exclude'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Field exclude'),
    '#options' => $ctypes,
    '#description' => t('Toggle content types which you don\'t want the field to appear in.'),
    '#default_value' => $field['exclude']
  );
  $form['code_identity']['code_code'] = array(
    '#type' => 'textarea',
    '#title' => t('Field code'),
    '#required' => TRUE,
    '#default_value' => $field['code']
  );

  _ds_field_object_info($form, $object);

  $form['code_identity']['code_submit'] = array(
    '#type' => 'submit',
    '#submit' => array('ds_code_field_form_submit'),
    '#value' => t('Save code field'),
  );

  $form['#field_type'] = ($field['type'] == DS_FIELD_OVERRIDABLE) ? DS_FIELD_OVERRIDDEN : (($field['type'] == DS_FIELD_OVERRIDDEN) ? DS_FIELD_OVERRIDDEN : DS_FIELD_CUSTOM);
  $form['#form_type'] = (empty($key)) ? 'insert' : 'update';
  $form['#module'] = $module;

  return $form;
}

/**
 * Return info about the value field, including token info.
 * @param array $form The current form.
 * @param string $object The object name (ie node, user, comment)
 */
function _ds_field_object_info(&$form, $object) {
  $form['code_identity']['help'] = array(
    '#type' => 'item',
    '#value' => t('Every field will be wrapped in &lt;div class="field field-key"&gt;VALUE&lt;/div&gt;. Enter PHP code between &lt;?php ?&gt;. Note that executing incorrect PHP-code can break your Drupal site.'),
  );

  if (module_exists('token')) {
    $form['code_identity']['help']['tokens'] = array(
      '#type' => 'fieldset',
      '#title' => t('Placeholder tokens'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['code_identity']['help']['tokens']['info'] = array(
      '#type' => 'item',
      '#value' => theme('token_help', $object),
    );
  }
  else {
    $form['code_identity']['help']['#value'] .= '<br />'. t("To use dynamic placeholder tokens in your custom fields (the ID or title of the current node, for example), download and install the <a href='@token'>Token module</a> from Drupal.org.", array('@token' => 'http://www.drupal.org/project/token'));
  }
}

/**
 * Validate code field submission, only on insert.
 */
function ds_code_field_form_validate($form, &$form_state) {
  if ($form['#form_type'] == 'insert') {

    $module = $form['#module'];
    $fields = variable_get($module .'_fields', array());
    $existing = array_keys($fields);

    if (in_array($form_state['values']['code_key'], $existing)) {
      form_set_error('code_key', t('This field already exists.'));
    }

    if (!preg_match('!^[a-z_]+$!', $form_state['values']['code_key'])) {
      form_set_error('code_key', t('The machine-readable name must contain only lowercase letters and underscores.'));
    }
  }
}

/**
 * Save code field.
 */
function ds_code_field_form_submit($form, &$form_state) {
  $module = $form['#module'];
  $fields = variable_get($module .'_fields', array());
  $fields[$form_state['values']['code_key']] = array(
    'title' => $form_state['values']['code_title'],
    'code' => $form_state['values']['code_code'],
    'exclude' => $form_state['values']['code_exclude'],
    'type' => $form['#field_type'],
  );
  variable_set($module .'_fields', $fields);
  $form_state['redirect'] = 'admin/ds/'. $form['#module'] .'/fields';
  drupal_set_message(t('Field %field has been saved.', array('%field' => $form_state['values']['title'])));
}

/**
 * Block field form.
 *
 * @param string $module The module the fields are for.
 * @param string $key The key of the field.
 * @param array $field The field with title and code keys.
 */
function ds_block_field_form($form_state, $module, $key = '', $field = array()) {
  $form = array();

  if (empty($field)) {
    $field = array('title' => '', 'block' => '', 'exclude' => array(), 'type' => DS_FIELD_BLOCK);
  }

  $form['block_identity'] = array(
    '#type' => 'fieldset',
    '#title' => (empty($key)) ? t('Add new block field'): t('Update block field'),
    '#collapsible' => (empty($key)) ? TRUE : FALSE,
    '#collapsed' => (empty($key)) ? TRUE : FALSE,
  );
  $form['block_identity']['block_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Field key'),
    '#description' => t('The machine-readable name of this field.'),
    '#required' => TRUE,
  );
  if (!empty($key)) {
    $form['block_identity']['block_key']['#disabled'] = TRUE;
    $form['block_identity']['block_key']['#value'] = $key;
    $form['block_identity']['block_key']['#description'] = t('The machine-readable name of this field. Note: you can not edit this field.');
  }
  $form['block_identity']['block_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Field title'),
    '#description' => t('The title to use when rendering the output and on the display administration screen.'),
    '#required' => TRUE,
    '#default_value' => $field['title']
  );
  $ctypes = node_get_types('names');
  $form['block_identity']['block_exclude'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Field exclude'),
    '#options' => $ctypes,
    '#description' => t('Toggle content types which you don\'t want the field to appear in.'),
    '#default_value' => $field['exclude']
  );

  $blocks = array();
  foreach (module_list() as $module) {
    $module_blocks = module_invoke($module, 'block', 'list');
    if ($module_blocks) {
      foreach ($module_blocks as $key => $info) {
        $blocks[ucfirst($module)][$module .'|'. $key] = $info['info'];
      }
    }
  }
  ksort($blocks);

  $form['block_identity']['block_block'] = array(
    '#type' => 'select',
    '#options' => $blocks,
    '#title' => t('Block'),
    '#required' => TRUE,
    '#default_value' => $field['block']
  );
  $form['block_identity']['block_render'] = array(
    '#type' => 'select',
    '#options' => array(
      BLOCK_TEMPLATE => t('Render through block template'),
      BLOCK_TITLE_CONTENT => t('Show block title + content'),
      BLOCK_CONTENT => t('Show only block content'),
    ),
    '#title' => t('Block render'),
    '#required' => TRUE,
    '#default_value' => $field['render']
  );

  $form['block_identity']['block_submit'] = array(
    '#type' => 'submit',
    '#submit' => array('ds_block_field_form_submit'),
    '#value' => t('Save block field'),
  );

  $form['#field_type'] = DS_FIELD_BLOCK;
  $form['#form_type'] = (empty($key)) ? 'insert' : 'update';
  $form['#module'] = $module;

  return $form;
}

/**
 * Validate block field submission, only on insert.
 */
function ds_block_field_form_validate($form, &$form_state) {
  if ($form['#form_type'] == 'insert') {

    $module = $form['#module'];
    $fields = variable_get($module .'_fields', array());
    $existing = array_keys($fields);

    if (in_array($form_state['values']['block_key'], $existing)) {
      form_set_error('block_key', t('This field already exists.'));
    }

    if (!preg_match('!^[a-z_]+$!', $form_state['values']['block_key'])) {
      form_set_error('block_key', t('The machine-readable name must contain only lowercase letters and underscores.'));
    }
  }
}

/**
 * Save block field.
 */
function ds_block_field_form_submit($form, &$form_state) {
  $module = $form['#module'];
  $fields = variable_get($module .'_fields', array());
  $fields[$form_state['values']['block_key']] = array(
    'title' => $form_state['values']['block_title'],
    'block' => $form_state['values']['block_block'],
    'exclude' => $form_state['values']['block_exclude'],
    'render' => $form_state['values']['block_render'],
    'type' => $form['#field_type'],
  );
  variable_set($module .'_fields', $fields);
  $form_state['redirect'] = 'admin/ds/'. $form['#module'] .'/fields';
  drupal_set_message(t('Field %field has been saved.', array('%field' => $form_state['values']['title'])));
}

/**
 * Field delete form.
 *
 * @param string $module The module the fields are for.
 * @param string $key The key of the field.
 * @param array $field The field with title and code keys.
 */
function ds_field_delete_form($form_state, $module, $key, $field) {
  $form = array();

  $action = ($field['type'] == DS_FIELD_OVERRIDDEN) ? t('reset') : t('delete');
  $form['question'] = array(
    '#type' => 'markup',
    '#value' => '<p>'. t('Are you sure you want to !action the field %field ?', array('!action' => $action, '%field' => $field['title'])) .'</p>',
  );
  $form['field'] = array(
    '#type' => 'value',
    '#value' => $key,
  );
  $form['button']['submit'] = array(
    '#prefix' => '<div>',
    '#type' => 'submit',
    '#value' => ($field['type'] == DS_FIELD_OVERRIDDEN) ? t('Reset') : t('Delete'),
  );
  $form['buttons']['cancel'] = array(
    '#suffix' => '</div>',
    '#type' => 'markup',
    '#value' => l('cancel', 'admin/ds/'. $module .'/fields'),
  );
  $form['#module'] = $module;
  return $form;
}

/**
 * Delete field.
 */
function ds_field_delete_form_submit($form, &$form_state) {
  $module = $form['#module'];
  $fields = variable_get($module .'_fields', array());
  $field = $fields[$form_state['values']['field']];
  $action = ($field['type'] == DS_FIELD_OVERRIDDEN) ? t('reset') : t('deleted');

  unset($fields[$form_state['values']['field']]);
  variable_set($module .'_fields', $fields);
  drupal_set_message(t('Field %field has been !action.', array('%field' => $form_state['values']['field'], '!action' => $action)));
  $form_state['redirect'] = 'admin/ds/'. $form['#module'] .'/fields';
}

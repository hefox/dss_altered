<?php
// $Id: theme.inc,v 1.1.2.1 2009/09/11 15:21:29 swentel Exp $

/**
 * @file
 * Theming functions for nd_cck.
 */

/**
 * Theme preprocess function for nd-cck-content-admin-field-overview-form.tpl.php.
 */
function template_preprocess_nd_cck_field_overview_form(&$vars) {
  $form = &$vars['form'];

  $order = _content_overview_order($form, $form['#field_rows'], $form['#group_rows']);
  $rows = array();

  // Identify the 'new item' keys in the form, they look like
  // _add_new_field, add_new_group.
  $keys = array_keys($form);
  $add_rows = array();
  foreach ($keys as $key) {
    if (drupal_substr($key, 0, 4) == '_add') {
      $add_rows[] = $key;
    }
  }
  while ($order) {
    $key = reset($order);
    $element = &$form[$key];

    // Only display the 'Add' separator if the 'add' rows are still
    // at the end of the table.
    if (!isset($added_separator)) {
      $remaining_rows = array_diff($order, $add_rows);
      if (empty($remaining_rows) && empty($element['#depth'])) {
        $row = new stdClass();
        $row->row_type = 'separator';
        $row->class = 'region';//'tabledrag-leaf region';
        $rows[] = $row;
        $added_separator = TRUE;
      }
    }

    $row = new stdClass();

    // Add target classes for the tabledrag behavior.
    $element['hidden_name']['#attributes']['class'] = 'field-name';
    // Add target classes for the update selects behavior.
    switch ($element['#row_type']) {
      case 'add_new_field':
        $element['type']['#attributes']['class'] = 'content-field-type-select';
        $element['widget_type']['#attributes']['class'] = 'content-widget-type-select';
        break;
      case 'add_existing_field':
        $element['field_name']['#attplatform_content_display_overview_formributes']['class'] = 'content-field-select';
        $element['widget_type']['#attributes']['class'] = 'content-widget-type-select';
        $element['label']['#attributes']['class'] = 'content-label-textfield';
        break;
    }
    foreach (element_children($element) as $child) {
      $row->{$child} = drupal_render($element[$child]);
    }
    $row->label_class = 'label-'. strtr($element['#row_type'], '_', '-');
    $row->row_type = $element['#row_type'];
    $row->indentation = theme('indentation', isset($element['#depth']) ? $element['#depth'] : 0);
    $row->class .= isset($element['#disabled_row']) ? ' menu-disabled' : '';
    $row->class .= isset($element['#add_new']) ? ' content-add-new' : '';
    $row->class .= isset($element['#leaf']) ? ' tabledrag-leaf' : '';
    $row->class .= isset($element['#root']) ? ' tabledrag-root' : '';

    $rows[] = $row;
    array_shift($order);
  }
  $vars['rows'] = $rows;
  $vars['submit'] = drupal_render($form);

  // Add settings for the update selects behavior.
  $js_fields = array();
  foreach (array_keys(content_existing_field_options($form['#type_name'])) as $field_name) {
    $field = content_fields($field_name);
    $js_fields[$field_name] = array('label' => $field['widget']['label'], 'type' => $field['type'], 'widget' => $field['widget']['type']);
  }
  drupal_add_js(array('contentWidgetTypes' => content_widget_type_options(), 'contentFields' => $js_fields), 'setting');
  drupal_add_js(drupal_get_path('module', 'content') .'/content.js');
}

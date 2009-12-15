<?php
// $Id: theme.inc,v 1.1.2.6 2009/11/27 15:53:46 swentel Exp $

/**
 * @file
 * Theming functions for ds.
 */

/**
 * Theme function to render the field content.
 *
 * @param string $content The content to render.
 * @param string $field_key The name of the field key.
 * @param array $field The current field.
 */
function theme_ds_field($content, $field_key, $field) {

  $output = '';
  if (!empty($content)) {
    if ($field['type'] == 'ds') {
      $output .= '<div class="field field-'. $field_key .'">';
      // Above label.
      if ($field['labelformat'] == 'above') {
        $output .= '<div class="field-label">'. $field['title'] .': </div>';
      }
      // Inline label
      if ($field['labelformat'] == 'inline') {
        $output .= '<div class="field-label-inline-first">'. $field['title'] .': </div>';
      }
      $output .= $content;
      $output .= '</div>';
    }
    else {
      $output = $content;
    }
  }

  return $output;
}

/**
 * Theme author name.
 */
function theme_ds_author_nolink(&$object) {
  $object->author_rendered = $object->name;
}

/**
 * Theme author name linked.
 */
function theme_ds_author_link(&$object) {
  $object->author_rendered = theme('username', $object);
}

/**
 * Template helper for theme_views_view_row_object().
 */
function template_preprocess_ds_row_object(&$vars) {

  $options = $vars['options'];
  $vars['object'] = ''; // make sure var is defined.

  switch ($vars['view']->base_table) {

    // Node support.
    case 'node':
      $nid = $vars['row']->nid;
      if (!is_numeric($nid)) {
        return;
      }

      $node = node_load($nid);
      if (empty($node)) {
        return;
      }

      $node->build_mode = $options['build_mode'];
      $vars['object'] = node_view($node, FALSE, FALSE, FALSE);
      break;

    // Comment support.
    // @todo fix this a bit more (option to theme with or without node
    // maybe even use our 'famous' threaded comments block.
    case 'comments':
      $cid = $vars['row']->cid;
      if (!is_numeric($nid)) {
        return;
      }

      $comment = _comment_load($cid);
      if (empty($comment)) {
        return;
      }

      $comment->build_mode = $options['build_mode'];
      //$vars['object'] = comment_view($comment);
      break;

    // User support.
    case 'users':
      $uid = $vars['row']->uid;
      if (!is_numeric($uid)) {
        return;
      }

      $account = user_load($uid);
      if (empty($account)) {
        return;
      }

      $account->build_mode = $options['build_mode'];
      user_build_content($account);

      // To theme user profiles, copy modules/user/user_profile.tpl.php
      // to your theme directory, and edit it as instructed in that file's comments.
      $vars['object'] = theme('user_profile', $account);
      break;
  }
}

/**
 * Theme preprocess function for ds-display-overview-form.tpl.php.
 */
function template_preprocess_ds_display_overview_form(&$vars) {
  $form = &$vars['form'];

  $build_mode = $form['#build_mode'];

  // Order the fields.
  $order = array();
  foreach ($form['#fields'] as $key => $field) {
    $order[$field] = $form[$field]['ds_weight']['#default_value'];
  }
  asort($order);

  // Render exclude build mode checkbox.
  $vars['exclude_build_mode'] = drupal_render($form['exclude_build_mode']);

  $rows = array();
  foreach ($order as $key => $field) {
    $element = &$form[$key];
    $row = new stdClass();

    // Each field will have a region, store it temporarily
    $region = $element[$build_mode]['region']['#default_value'];

    foreach (element_children($element) as $child) {

      // Render the display fields
      if ($child == $build_mode) {
        $row->{$child}->label = drupal_render($element[$child]['label']);
        $row->{$child}->format = drupal_render($element[$child]['format']);
        $row->{$child}->region = drupal_render($element[$child]['region']);
      }
      // Render the rest of the fields
      else {
        // Process weight.
        if ($child == 'ds_weight') {
          $element['ds_weight']['#attributes']['class'] = 'field-weight field-weight-'. $region;
          $element['ds_weight'] = process_weight($element['ds_weight']);
        }
        $row->{$child} = drupal_render($element[$child]);
      }
    }

    // Add draggable.
    $row->class = 'draggable';
    if ($region == 'disabled') {
      $row->class .= ' region-css-'. $region;
    }

    $row->label_class = 'label-field';
    $rows[$region][] = $row;
  }

  drupal_add_js('misc/tableheader.js');
  drupal_add_js(drupal_get_path('module', 'ds') .'/js/ds.js');
  drupal_add_css(drupal_get_path('module', 'ds') .'/css/ds.css');

  $regions = ds_regions(variable_get('ds_build_mode_'. $build_mode, 'all'));
  foreach ($regions as $region => $title) {
    drupal_add_tabledrag('fields', 'match', 'sibling', 'field-region-select', 'field-region-'. $region, NULL, FALSE);
    // We need to have subgroups to make the dragging available within parents
    drupal_add_tabledrag('fields', 'order', 'sibling', 'field-weight', 'field-weight-'. $region);
  }

  // Plugins available.
  if ($form['#plugins'] == TRUE) {
    $vars['plugins'] = TRUE;
    $vars['plugins_content'] = '';
    foreach ($form['#plugin_keys'] as $key) {
      $vars['plugins_content'] .= drupal_render($form[$key]);
    }
  }

  $vars['rows'] = $rows;
  $vars['submit'] = drupal_render($form);
  $vars['regions'] = $regions;
  $vars['build_mode'] = $build_mode;
  $vars['extra_style'] = ($form['exclude_build_mode']['#default_value'] == TRUE) ? 'style="display: none;"' : '';
}

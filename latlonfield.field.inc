<?php

/**
 * @file
 * Implement a latlon field.
 */

/**
 * Implements hook_field_info().
 */
function latlonfield_field_info() {
  return array(
    'latlon' => array(
      'label' => t('Lat/Lon'),
      'description' => t('This field stores a latitude and a longitude.'),
      'settings' => array(),
      'instance_settings' => array(),
      'default_widget' => 'latlon',
      'default_formatter' => 'latlon',
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function latlonfield_field_is_empty($item, $field) {
  return empty($item['latitude']) || empty($item['longitude']);
}

/**
 * Implements hook_field_widget_info().
 */
function latlonfield_field_widget_info() {
  return array(
    'latlon' => array(
      'label' => t('Default'),
      'field types' => array('latlon'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function latlonfield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $element += array(
    '#type' => 'fieldset',
    '#title' => t('Latitude/Longitude'),
    '#collapsible' => FALSE,
    '#element_validate' => array('latlonfield_element_validate'),
  );
  $element['latitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Latitude'),
    '#default_value' => isset($items[$delta]['latitude']) ? $items[$delta]['latitude'] : '',
    '#element_validate' => array('element_validate_number'),
    '#pre_render' => array('latlonfield_element_required'),
  );
  $element['longitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Longitude'),
    '#default_value' => isset($items[$delta]['longitude']) ? $items[$delta]['longitude'] : '',
    '#element_validate' => array('element_validate_number'),
    '#pre_render' => array('latlonfield_element_required'),
  );

  return $element;
}

/**
 * Pre-render element callback to theme required element.
 */
function latlonfield_element_required($element) {
  $element['#required'] = TRUE;
  return $element;
}

/**
 * Address element validate callback.
 */
function latlonfield_element_validate($element, &$form_state, $form) {

  if (!empty($element['latitude']['#value']) && empty($element['longitude']['#value'])) {
    form_error($element['longitude'], t('Field %name is required.', array('%name' => $element['longitude']['#title'])));
  }
  elseif (!empty($element['latitude']['#value']) && empty($element['longitude']['#value'])) {
    form_error($element['latitude'], t('Field %name is required.', array('%name' => $element['latitude']['#title'])));
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function latlonfield_field_formatter_info() {
  $formatters = array(
    'latlon' => array(
      'label' => t('Default'),
      'field types' => array('latlon'),
      'settings' => array(),
    ),
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_view().
 */
function latlonfield_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  foreach ($items as $delta => $item) {
    $element[$delta] = array(
      '#theme' => 'latlonfield_formatter',
      '#item' => $item,
    );
  }

  return $element;
}

/**
 * Returns HTML for a default latlon field formatter (no map).
 *
 * @ingroup themeable
 */
function template_latlonfield_formatter($variables) {
  $item = $variables['item'];
  $output = '';

  $output .= '<div>';
  $output .= '<div class="latitude">' . $item['latitude'] . '</div>';
  $output .= '<div class="longitude">' . $item['longitude'] . '</div>';
  $output .= '</div>';

  return $output;
}

<?php

/**
 * Implements hook_field_widget_info().
 */
function latlonfield_field_widget_info() {
  return [
    'latlon' => [
      'label'       => t('Default'),
      'field types' => ['latlon'],
      'settings'    => [],
    ],
    'address_fr_afnor' => [
      'label'       => t('French SNA/AFNOR standard'),
      'field types' => ['address'],
      'settings'    => [],
    ],
  ];
}

/**
 * Core is too strict with number validation
 */
function element_validate_number_with_trim($element, &$form_state) {
  $value = trim($element['#value']);
  if ($value != '' && !is_numeric($value)) {
    form_error($element, t('%name must be a number.', array('%name' => $element['#title'])));
  }
  form_set_value($element, $value, $form_state);
}

/**
 * Not really implements hook_field_widget_form().
 */
function _latlonfield_field_widget_form_latlon(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  if (!isset($element['#required'])) {
    $element['#required'] = false;
  }
  $required = $element['#required'];

  $element['latitude'] = [
    '#type'             => 'textfield',
    '#title'            => t('Latitude'),
    '#default_value'    => isset($items[$delta]['latitude']) ? $items[$delta]['latitude'] : '',
    '#element_validate' => ['element_validate_number_with_trim'],
    '#required'         => $required,
    '#pre_render'       => $required ? ['latlonfield_element_required'] : [],
  ];
  $element['longitude'] = [
    '#type'             => 'textfield',
    '#title'            => t('Longitude'),
    '#default_value'    => isset($items[$delta]['longitude']) ? $items[$delta]['longitude'] : '',
    '#element_validate' => ['element_validate_number_with_trim'],
    '#required'         => $required,
    '#pre_render'       => $required ? ['latlonfield_element_required'] : [],
  ];

  return $element;
}

/**
 * Not really implements hook_field_widget_form().
 */
function _latlonfield_field_widget_form_address_fr_afnor(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  if (!isset($element['#required'])) {
    $element['#required'] = false;
  }
  $required = $element['#required'];

  $item = isset($items[$delta]) ? $items[$delta] : [];
  $item += [
    'name_line'           => '',
    'organisation_name'   => '',
    'premise'             => '',
    'thoroughfare'        => '',
    'dependent_locality'  => '',
    'postal_code'         => '',
    'locality'            => '',
    'country'             => 'fr', // @todo should be instance setting instead
  ];

  $element['name_line'] = [
    '#title'            => "Raison sociale/dénomination", // FIXME: Hardcoded french
    '#type'             => 'textfield',
    '#size'             => 255,
    '#maxlength'        => 255,
    '#default_value'    => $item['name_line'],
  ];
  $element['organisation_name'] = [
    '#title'            => "Destinataire ou service", // FIXME: Hardcoded french
    '#type'             => 'textfield',
    '#size'             => 255,
    '#maxlength'        => 255,
    '#default_value'    => $item['organisation_name'],
  ];
  $element['premise'] = [
    '#title'            => t("Address 1"),
    '#type'             => 'textfield',
    '#size'             => 255,
    '#maxlength'        => 255,
    '#default_value'    => $item['premise'],
  ];
  $element['thoroughfare'] = [
    '#title'            => t("Address 2"),
    '#type'             => 'textfield',
    '#size'             => 255,
    '#maxlength'        => 255,
    '#default_value'    => $item['thoroughfare'],
  ];
  $element['dependent_locality'] = [
    '#title'            => "Mention spéciale", // FIXME: Hardcoded french
    '#type'             => 'textfield',
    '#size'             => 255,
    '#maxlength'        => 255,
    '#default_value'    => $item['dependent_locality'],
  ];
  $element['postal_code'] = [
    '#title'            => t("Postal code"),
    '#type'             => 'textfield',
    '#size'             => 10,
    '#maxlength'        => 10,
    '#default_value'    => $item['postal_code'],
    '#required'         => $required,
    '#pre_render'       => $required ? ['latlonfield_element_required'] : [],
  ];
  $element['locality'] = [
    '#title'            => t("City"),
    '#type'             => 'textfield',
    '#size'             => 255,
    '#maxlength'        => 255,
    '#attributes'       => ['class' => ['addressfield-city']],
    '#default_value'    => $item['locality'],
    '#required'         => $required,
    '#pre_render'       => $required ? ['latlonfield_element_required'] : [],
  ];
  $element['country'] = [
    '#title'            => t('Country'),
    '#type'             => 'select',
    '#options'          => ['fr' => "FRANCE"], // @todo coutry
    '#default_value'    => $item['country'],
    '#disabled'         => true,
    '#required'         => $required,
    '#pre_render'       => $required ? ['latlonfield_element_required'] : [],
  ];

  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function latlonfield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  switch ($instance['widget']['type']) {

    case 'latlon':
      return _latlonfield_field_widget_form_latlon($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);

    case 'address_fr_afnor':
      return _latlonfield_field_widget_form_address_fr_afnor($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  }

  return $element;
}

/**
 * Pre-render element callback to theme required element.
 */
function latlonfield_element_required($element) {
  $element['#required'] = true;
  return $element;
}

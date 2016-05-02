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
 * Not really implements hook_field_widget_form().
 */
function _latlonfield_field_widget_form_latlon(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $element['latitude'] = [
    '#type'             => 'textfield',
    '#title'            => t('Latitude'),
    '#default_value'    => isset($items[$delta]['latitude']) ? $items[$delta]['latitude'] : '',
    '#element_validate' => ['element_validate_number'],
    '#pre_render' => ['latlonfield_element_required'],
  ];
  $element['longitude'] = [
    '#type'             => 'textfield',
    '#title'            => t('Longitude'),
    '#default_value'    => isset($items[$delta]['longitude']) ? $items[$delta]['longitude'] : '',
    '#element_validate' => ['element_validate_number'],
    '#pre_render'       => ['latlonfield_element_required'],
  ];

  return $element;
}

/**
 * Not really implements hook_field_widget_form().
 */
function _latlonfield_field_widget_form_address_fr_afnor(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

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
    '#size'             => 38,
    '#maxlength'        => 38,
    '#default_value'    => $item['name_line'],
  ];
  $element['organisation_name'] = [
    '#title'            => "Destinataire ou service", // FIXME: Hardcoded french
    '#type'             => 'textfield',
    '#size'             => 38,
    '#maxlength'        => 38,
    '#default_value'    => $item['organisation_name'],
  ];
  $element['premise'] = [
    '#title'            => t("Address 1"),
    '#type'             => 'textfield',
    '#size'             => 38,
    '#maxlength'        => 38,
    '#default_value'    => $item['premise'],
  ];
  $element['thoroughfare'] = [
    '#title'            => t("Address 2"),
    '#type'             => 'textfield',
    '#size'             => 38,
    '#maxlength'        => 38,
    '#default_value'    => $item['thoroughfare'],
  ];
  $element['dependent_locality'] = [
    '#title'            => "Mention spéciale", // FIXME: Hardcoded french
    '#type'             => 'textfield',
    '#size'             => 38,
    '#maxlength'        => 38,
    '#default_value'    => $item['dependent_locality'],
  ];
  $element['postal_code'] = [
    '#title'            => t("Postal code"),
    '#type'             => 'textfield',
    '#size'             => 10,
    '#maxlength'        => 10,
    '#default_value'    => $item['postal_code'],
    '#pre_render'       => ['latlonfield_element_required'],
  ];
  $element['locality'] = [
    '#title'            => t("City"),
    '#type'             => 'textfield',
    '#size'             => 27,
    '#maxlength'        => 27,
    '#attributes'       => ['class' => ['addressfield-city']],
    '#default_value'    => $item['locality'],
    '#pre_render'       => ['latlonfield_element_required'],
  ];
  $element['country'] = [
    '#title'            => t('Country'),
    '#type'             => 'select',
    '#options'          => ['fr' => "FRANCE"], // @todo coutry
    '#default_value'    => $item['country'],
    '#disabled'         => true,
    '#pre_render'       => ['latlonfield_element_required'],
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

/**
 * Address element validate callback.
 */
function latlonfield_element_validate($element, &$form_state, $form) {
  if (!empty($element['latitude']['#value']) && empty($element['longitude']['#value'])) {
    form_error($element['longitude'], t('Field %name is required.', ['%name' => $element['longitude']['#title']]));
  } else if (!empty($element['latitude']['#value']) && empty($element['longitude']['#value'])) {
    form_error($element['latitude'], t('Field %name is required.', ['%name' => $element['latitude']['#title']]));
  }
}

// Type definitions for Minecraft Bedrock Edition script APIs
// Project: https://docs.microsoft.com/minecraft/creator/
// Definitions by: Jake Shirley <https://github.com/JakeShirley>
//                 Mike Ammerlaan <https://github.com/mammerla>

/* *****************************************************************************
   Copyright (c) Microsoft Corporation.
   ***************************************************************************** */
/**
 * @beta
 * @packageDocumentation
 * `@minecraft/server-ui` 模块包含了用于基于对话框的简易用户交互功能的类型。
 * 
 *   * {@link ActionFormData} 包含了一系列带有标题与图片的按钮。便于要求用户从一系列选项中选择。
 *   * {@link MessageFormData} 则是包含了两个按钮的简易消息框，允许用户选择是/否或者确定/取消。
 *   * {@link ModalFormData} 则提供了一套更具灵活性的类似问卷形式的控件，允许用户填写并提交。
 * 
 * The `@minecraft/server-ui` module contains types for
 * expressing simple dialog-based user experiences.
 *
 *   * {@link ActionFormData} contain a list of buttons with
 * captions and images that can be used for presenting a set of
 * options to a player.
 *   * {@link MessageFormData} are simple two-button message
 * experiences that are functional for Yes/No or OK/Cancel
 * questions.
 *   * {@link ModalFormData} allow for a more flexible
 * "questionnaire-style" list of controls that can be used to
 * take input.
 *
 * Manifest Details
 * ```json
 * {
 *   "module_name": "@minecraft/server-ui",
 *   "version": "2.2.0-beta"
 * }
 * ```
 *
 */
import { EngineError } from '@minecraft/common';
import { InvalidEntityError, Player, RawMessage, RawMessageError } from '@minecraft/server';
/**
 * The reason why a data driven UI screen (MessageBox or
 * CustomForm) was closed.
 */
export enum DataDrivenScreenClosedReason {
    /**
     * @remarks
     * The screen was closed by the client (e.g., the player
     * dismissed it or there was a client authoritative close
     * button).
     *
     */
    ClientClosed = 'ClientClosed',
    /**
     * @remarks
     * The screen was closed by the server, likely by the close
     * API.
     *
     */
    ServerClosed = 'ServerClosed',
    /**
     * @remarks
     * The screen could not be shown because the player was busy
     * with another UI interaction.
     *
     */
    UserBusy = 'UserBusy',
}

/**
 * An enum representing the reasons why a form was canceled.
 */
export enum FormCancelationReason {
    /**
     * @remarks
     * The form was canceled because the player was busy with
     * another UI interaction.
     *
     */
    UserBusy = 'UserBusy',
    /**
     * @remarks
     * The form was canceled because the player closed it.
     *
     */
    UserClosed = 'UserClosed',
}

/**
 * An enum representing the reasons why a form was rejected.
 */
export enum FormRejectReason {
    /**
     * @remarks
     * The form was rejected because the response from the client
     * was malformed or could not be parsed.
     *
     */
    MalformedResponse = 'MalformedResponse',
    /**
     * @remarks
     * The form was rejected because the player quit the game
     * before responding.
     *
     */
    PlayerQuit = 'PlayerQuit',
    /**
     * @remarks
     * The form was rejected because the server shut down before
     * the player responded.
     *
     */
    ServerShutdown = 'ServerShutdown',
}

/**
 * The reason why a form visibility operation failed.
 */
export enum FormVisibilityErrorReason {
    /**
     * @remarks
     * The operation failed because the form is already being shown
     * to the player.
     *
     */
    AlreadyShowing = 'AlreadyShowing',
    /**
     * @remarks
     * The operation failed because the form is not currently being
     * shown to the player.
     *
     */
    NotShowing = 'NotShowing',
}

/**
 * An enum representing the errors that can occur during text
 * filtering. This is used to provide more context about the
 * filtering process.
 */
export enum TextFilteringError {
    /**
     * @remarks
     * The text was not filtered because the player disabled text
     * filtering in their settings.
     *
     */
    DisabledByPlayer = 'DisabledByPlayer',
    /**
     * @remarks
     * The text was not filtered because the service is
     * unreachable. This can occur if there are network issues or
     * if the service is down for maintenance.
     *
     */
    TextProcessorServiceUnreachable = 'TextProcessorServiceUnreachable',
    /**
     * @remarks
     * An unknown error occurred during text filtering. This can
     * occur if there is an unexpected issue with the text
     * filtering service or if the service returns an error that is
     * not categorized under the other error types.
     *
     */
    Unknown = 'Unknown',
}

/**
 * Builds a simple player form with buttons that let the player
 * take action.
 * @seeExample showActionForm.ts
 * @seeExample showFavoriteMonth.ts
 */
export class ActionFormData {
    /**
     * @remarks
     * Method that sets the body text for the modal form.
     *
     */
    body(bodyText: RawMessage | string): ActionFormData;
    /**
     * @remarks
     * Adds a button to this form with an icon from a resource
     * pack.
     *
     */
    button(text: RawMessage | string, iconPath?: string): ActionFormData;
    /**
     * @remarks
     * Adds a section divider to the form.
     *
     */
    divider(): ActionFormData;
    /**
     * @remarks
     * Adds a header to the form.
     *
     * @param text
     * Text to display.
     */
    header(text: RawMessage | string): ActionFormData;
    /**
     * @remarks
     * Adds a label to the form.
     *
     * @param text
     * Text to display.
     */
    label(text: RawMessage | string): ActionFormData;
    /**
     * @remarks
     * Creates and shows this modal popup form. Returns
     * asynchronously when the player confirms or cancels the
     * dialog.
     *
     * @worldMutation
     *
     * @param player
     * Player to show this dialog to.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     */
    show(player: Player): Promise<ActionFormResponse>;
    /**
     * @remarks
     * This builder method sets the title for the modal dialog.
     *
     */
    title(titleText: RawMessage | string): ActionFormData;
}

/**
 * Returns data about the player results from a modal action
 * form.
 * @seeExample showActionForm.ts
 * @seeExample showFavoriteMonth.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ActionFormResponse extends FormResponse {
    private constructor();
    /**
     * @remarks
     * Returns the index of the button that was pushed.
     *
     */
    readonly selection?: number;
}

/**
 * A customizable data driven (DDUI) form that lets you add
 * buttons, labels, toggles, dropdowns, sliders, text fields,
 * and more. The form layout is built by calling methods to add
 * components before calling show(). Any Observable values
 * bound to form components will automatically update the UI
 * when their values change.
 */
export class CustomForm {
    /**
     * @remarks
     * Creates a new CustomForm for the specified player with the
     * given title.
     *
     * @param player
     * The player to show this form to.
     * @param title
     * The title text to display at the top of the form.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    constructor(
        player: Player,
        title: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
    );
    /**
     * @remarks
     * Adds a clickable button to the form layout. Returns the form
     * instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label to display on the button.
     * @param onClick
     * A callback function that is invoked when the player clicks
     * the button.
     * @param options
     * Optional configuration for the button, such as a tooltip,
     * disabled state, or visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    button(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        onClick: () => void,
        options?: ButtonOptions,
    ): CustomForm;
    /**
     * @remarks
     * Closes the form if it is currently being shown to the
     * player. Throws a FormVisibilityError if the form is not
     * currently open.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link FormVisibilityError}
     *
     * {@link InvalidEntityError}
     */
    close(): void;
    /**
     * @remarks
     * Adds a close button to the form at the bottom and as an 'X'
     * in the corner. Returns the form instance to allow method
     * chaining.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    closeButton(): CustomForm;
    /**
     * @remarks
     * Adds a horizontal divider line to the form layout. Useful
     * for visually separating sections of the form. Returns the
     * form instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param options
     * Optional configuration for the divider, such as visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    divider(options?: DividerOptions): CustomForm;
    /**
     * @remarks
     * Adds a dropdown selection control to the form layout. The
     * current selection is tracked via an ObservableNumber and
     * will update when the player changes the selection. Returns
     * the form instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label displayed around the dropdown.
     * @param value
     * An ObservableNumber that holds the index of the currently
     * selected item.
     * @param items
     * The list of items to display in the dropdown.
     * @param options
     * Optional configuration for the dropdown, such as a
     * description, disabled state, or visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     *
     * {@link InvalidObservableError}
     */
    dropdown(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        value: ObservableNumber,
        items: DropdownItemData[],
        options?: DropdownOptions,
    ): CustomForm;
    /**
     * @remarks
     * Adds a header text component to the form layout. Headers are
     * displayed in a larger or bolder style than regular labels,
     * and are suitable for section titles. Returns the form
     * instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param text
     * The header text to display.
     * @param options
     * Optional configuration for the header, such as visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    header(text: ObservableString | ObservableUIRawMessage | string | UIRawMessage, options?: TextOptions): CustomForm;
    /**
     * @beta
     * @remarks
     * Adds an image component to the form layout.
     *
     * @worldMutation
     *
     * @param src
     * The image source path to display. Must be a relative path to
     * an image file within the provided pack.
     * @param pack
     * The resource pack identifier that contains the provided
     * image source.
     * @param options
     * Optional configuration for the image, such as visibility or
     * width.
     * @returns
     * The form instance to allow method chaining.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     *
     * {@link InvalidObservableError}
     */
    image(src: ObservableString | string, pack: ObservableString | string, options?: ImageOptions): CustomForm;
    /**
     * @remarks
     * Returns true if the form is currently being shown to the
     * player, false otherwise.
     *
     * @worldMutation
     *
     */
    isShowing(): boolean;
    /**
     * @remarks
     * Adds a read-only text label to the form layout. Returns the
     * form instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param text
     * The text to display in the label.
     * @param options
     * Optional configuration for the label, such as visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    label(text: ObservableString | ObservableUIRawMessage | string | UIRawMessage, options?: TextOptions): CustomForm;
    /**
     * @remarks
     * Shows the form to the player. Returns a promise that
     * resolves with a DataDrivenScreenClosedReason indicating how
     * the form was closed.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link FormVisibilityError}
     *
     * {@link InvalidEntityError}
     */
    show(): Promise<DataDrivenScreenClosedReason>;
    /**
     * @remarks
     * Adds a numeric slider control to the form layout. The
     * current value is tracked via an ObservableNumber and will
     * update as the player moves the slider. Returns the form
     * instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label displayed around the slider.
     * @param value
     * An ObservableNumber that holds the current value of the
     * slider.
     * @param min
     * The minimum value of the slider range.
     * @param max
     * The maximum value of the slider range.
     * @param options
     * Optional configuration for the slider, such as step size, a
     * description, disabled state, or visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     *
     * {@link InvalidObservableError}
     */
    slider(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        value: ObservableNumber,
        min: number | ObservableNumber,
        max: number | ObservableNumber,
        options?: SliderOptions,
    ): CustomForm;
    /**
     * @remarks
     * Adds a vertical spacer component to the form layout. Useful
     * for adding empty space between form components. Returns the
     * form instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param options
     * Optional configuration for the spacer, such as visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    spacer(options?: SpacingOptions): CustomForm;
    /**
     * @remarks
     * Adds a text input field to the form layout. The current text
     * value is tracked via an ObservableString and will update as
     * the player types. Returns the form instance to allow method
     * chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label displayed around the text field.
     * @param text
     * An ObservableString that holds the current text value of the
     * input field.
     * @param options
     * Optional configuration for the text field, such as a
     * description, disabled state, or visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     *
     * {@link InvalidObservableError}
     */
    textField(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        text: ObservableString,
        options?: TextFieldOptions,
    ): CustomForm;
    /**
     * @remarks
     * Adds a toggle (on/off switch) control to the form layout.
     * The current state is tracked via an ObservableBoolean and
     * will update when the player toggles it. Returns the form
     * instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label displayed next to the toggle.
     * @param toggled
     * An ObservableBoolean that holds the current on/off state of
     * the toggle.
     * @param options
     * Optional configuration for the toggle, such as a
     * description, disabled state, or visibility.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     *
     * {@link InvalidObservableError}
     */
    toggle(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        toggled: ObservableBoolean,
        options?: ToggleOptions,
    ): CustomForm;
}

/**
 * Base type for a form response.
 */
export class FormResponse {
    private constructor();
    /**
     * @remarks
     * Contains additional details as to why a form was canceled.
     *
     */
    readonly cancelationReason?: FormCancelationReason;
    /**
     * @remarks
     * If true, the form was canceled by the player (e.g., they
     * selected the pop-up X close button).
     *
     */
    readonly canceled: boolean;
}

/**
 * A simple message form with two buttons and a text body. Use
 * this class to show a basic dialog to a player and handle the
 * player's button selection.
 */
export class MessageBox {
    /**
     * @remarks
     * Creates a new MessageBox for the specified player with the
     * given title.
     *
     * @param player
     * The player to show this message box to.
     * @param title
     * The title text to display at the top of the message box.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    constructor(
        player: Player,
        title: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
    );
    /**
     * @remarks
     * Sets the body text displayed in the message box. Returns the
     * message box instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param body
     * The body text to display. Accepts either a plain string or
     * an ObservableString.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    body(body: ObservableString | ObservableUIRawMessage | string | UIRawMessage): MessageBox;
    /**
     * @remarks
     * Sets the label for the first button of the message box.
     * Returns the message box instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label to display on the first button.
     * @param tooltip
     * Optional tooltip text shown when hovering over the first
     * button.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    button1(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        tooltip?: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
    ): MessageBox;
    /**
     * @remarks
     * Sets the label for the second button of the message box.
     * Returns the message box instance to allow method chaining.
     *
     * @worldMutation
     *
     * @param label
     * The text label to display on the second button.
     * @param tooltip
     * Optional tooltip text shown when hovering over the second
     * button.
     * @throws This function can throw errors.
     *
     * {@link InvalidFormModificationError}
     */
    button2(
        label: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
        tooltip?: ObservableString | ObservableUIRawMessage | string | UIRawMessage,
    ): MessageBox;
    /**
     * @remarks
     * Closes the message box if it is currently being shown to the
     * player. Throws a FormVisibilityError if the form is not
     * currently open.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link FormVisibilityError}
     *
     * {@link InvalidEntityError}
     */
    close(): void;
    /**
     * @remarks
     * Returns true if the message box is currently being shown to
     * the player, false otherwise.
     *
     * @worldMutation
     *
     */
    isShowing(): boolean;
    /**
     * @remarks
     * Shows the message box to the player. Returns a promise that
     * resolves with a MessageBoxResult containing the close reason
     * and the player's button selection.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link FormVisibilityError}
     *
     * {@link InvalidEntityError}
     */
    show(): Promise<MessageBoxResult>;
}

/**
 * Builds a simple two-button modal dialog.
 * @seeExample showBasicMessageForm.ts
 * @seeExample showTranslatedMessageForm.ts
 */
export class MessageFormData {
    /**
     * @remarks
     * Method that sets the body text for the modal form.
     *
     */
    body(bodyText: RawMessage | string): MessageFormData;
    /**
     * @remarks
     * Method that sets the text for the first button of the
     * dialog.
     *
     */
    button1(text: RawMessage | string): MessageFormData;
    /**
     * @remarks
     * This method sets the text for the second button on the
     * dialog.
     *
     */
    button2(text: RawMessage | string): MessageFormData;
    /**
     * @remarks
     * Creates and shows this modal popup form. Returns
     * asynchronously when the player confirms or cancels the
     * dialog.
     *
     * @worldMutation
     *
     * @param player
     * Player to show this dialog to.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     */
    show(player: Player): Promise<MessageFormResponse>;
    /**
     * @remarks
     * This builder method sets the title for the modal dialog.
     *
     */
    title(titleText: RawMessage | string): MessageFormData;
}

/**
 * Returns data about the player results from a modal message
 * form.
 * @seeExample showBasicMessageForm.ts
 * @seeExample showTranslatedMessageForm.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class MessageFormResponse extends FormResponse {
    private constructor();
    /**
     * @remarks
     * Returns the index of the button that was pushed.
     *
     */
    readonly selection?: number;
}

/**
 * Used to create a fully customizable pop-up form for a
 * player.
 * @seeExample showBasicModalForm.ts
 */
export class ModalFormData {
    /**
     * @remarks
     * Adds a section divider to the form.
     *
     */
    divider(): ModalFormData;
    /**
     * @remarks
     * Adds a dropdown with choices to the form.
     *
     * @param label
     * The label to display for the dropdown.
     * @param items
     * The selectable items for the dropdown.
     * @param dropdownOptions
     * The optional additional values for the dropdown creation.
     */
    dropdown(
        label: RawMessage | string,
        items: (RawMessage | string)[],
        dropdownOptions?: ModalFormDataDropdownOptions,
    ): ModalFormData;
    /**
     * @remarks
     * Adds a header to the form.
     *
     * @param text
     * Text to display.
     */
    header(text: RawMessage | string): ModalFormData;
    /**
     * @remarks
     * Adds a label to the form.
     *
     * @param text
     * Text to display.
     */
    label(text: RawMessage | string): ModalFormData;
    /**
     * @remarks
     * Creates and shows this modal popup form. Returns
     * asynchronously when the player confirms or cancels the
     * dialog.
     *
     * @worldMutation
     *
     * @param player
     * Player to show this dialog to.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     */
    show(player: Player): Promise<ModalFormResponse>;
    /**
     * @remarks
     * Adds a numeric slider to the form.
     *
     * @param label
     * The label to display for the slider.
     * @param minimumValue
     * The minimum selectable possible value.
     * @param maximumValue
     * The maximum selectable possible value.
     * @param sliderOptions
     * The optional additional values for the slider creation.
     */
    slider(
        label: RawMessage | string,
        minimumValue: number,
        maximumValue: number,
        sliderOptions?: ModalFormDataSliderOptions,
    ): ModalFormData;
    submitButton(submitButtonText: RawMessage | string): ModalFormData;
    /**
     * @remarks
     * Adds a textbox to the form.
     *
     * @param label
     * The label to display for the textfield.
     * @param placeholderText
     * The place holder text to display.
     * @param textFieldOptions
     * The optional additional values for the textfield creation.
     */
    textField(
        label: RawMessage | string,
        placeholderText: RawMessage | string,
        textFieldOptions?: ModalFormDataTextFieldOptions,
    ): ModalFormData;
    /**
     * @remarks
     * This builder method sets the title for the modal dialog.
     *
     */
    title(titleText: RawMessage | string): ModalFormData;
    /**
     * @remarks
     * Adds a toggle checkbox button to the form.
     *
     * @param label
     * The label to display for the toggle.
     * @param toggleOptions
     * The optional additional values for the toggle creation.
     */
    toggle(label: RawMessage | string, toggleOptions?: ModalFormDataToggleOptions): ModalFormData;
}

/**
 * Returns data about player responses to a modal form.
 * @seeExample showBasicModalForm.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ModalFormResponse extends FormResponse {
    private constructor();
    /**
     * @remarks
     * An ordered set of values based on the order of controls
     * specified by ModalFormData.
     *
     */
    readonly formValues?: (boolean | number | string | undefined)[];
}

/**
 * An observable that holds a boolean value. Listeners are
 * notified whenever the value changes.
 */
export class ObservableBoolean {
    /**
     * @remarks
     * Creates a new ObservableBoolean with the provided initial
     * boolean value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The initial boolean value for this observable.
     * @param options
     * Optional configuration for the observable, such as whether
     * the value can be written by the client.
     */
    constructor(data: boolean, options?: ObservableOptions);
    /**
     * @remarks
     * Returns the current boolean value held by this observable.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    getData(): boolean;
    /**
     * @remarks
     * Updates the boolean value held by this observable. If the
     * new value differs from the current value, all subscribed
     * listeners are notified with the new value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The new boolean value to set.
     */
    setData(data: boolean): void;
    /**
     * @remarks
     * Registers a callback to be invoked whenever the observable's
     * value changes. Returns the callback, which can be passed to
     * unsubscribe to remove the listener.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * A function that receives the new boolean value each time the
     * observable changes.
     */
    subscribe(callback: (arg0: boolean) => void): (arg0: boolean) => void;
    /**
     * @remarks
     * Removes a previously registered listener from this
     * observable. Returns true if the listener was found and
     * removed, false if it was not found.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * The callback handle previously returned by subscribe.
     */
    unsubscribe(callback: (arg0: boolean) => void): boolean;
}

/**
 * An observable that holds a numeric value. Listeners are
 * notified whenever the value changes.
 */
export class ObservableNumber {
    /**
     * @remarks
     * Creates a new ObservableNumber with the provided initial
     * numeric value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The initial numeric value for this observable.
     * @param options
     * Optional configuration for the observable, such as whether
     * the value can be written by the client.
     */
    constructor(data: number, options?: ObservableOptions);
    /**
     * @remarks
     * Returns the current numeric value held by this observable.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    getData(): number;
    /**
     * @remarks
     * Updates the numeric value held by this observable. If the
     * new value differs from the current value, all subscribed
     * listeners are notified with the new value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The new numeric value to set.
     */
    setData(data: number): void;
    /**
     * @remarks
     * Registers a callback to be invoked whenever the observable's
     * value changes. Returns the callback, which can be passed to
     * unsubscribe to remove the listener.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * A function that receives the new numeric value each time the
     * observable changes.
     */
    subscribe(callback: (arg0: number) => void): (arg0: number) => void;
    /**
     * @remarks
     * Removes a previously registered listener from this
     * observable. Returns true if the listener was found and
     * removed, false if it was not found.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * The callback handle previously returned by subscribe.
     */
    unsubscribe(callback: (arg0: number) => void): boolean;
}

/**
 * An observable that holds a string value. Listeners are
 * notified whenever the value changes.
 */
export class ObservableString {
    /**
     * @remarks
     * Creates a new ObservableString with the provided initial
     * string value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The initial string value for this observable.
     * @param options
     * Optional configuration for the observable, such as whether
     * the value can be written by the client.
     */
    constructor(data: string, options?: ObservableOptions);
    /**
     * @remarks
     * Returns the current string value held by this observable.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    getData(): string;
    /**
     * @remarks
     * Gets filtered data from the Observable (only available for
     * strings). In case of failure, it will return an array of
     * TextFilteringError that can provide more context about the
     * filtering process. For testing purposes, the options are
     * available under 'Creator -> Text Filtering' settings menu.
     * This delay is only applied to the getFilteredText function
     * and can be used to simulate network latency when testing.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    getFilteredText(player: Player): Promise<TextFilteringError[] | string>;
    /**
     * @remarks
     * Updates the string value held by this observable. If the new
     * value differs from the current value, all subscribed
     * listeners are notified with the new value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The new string value to set.
     */
    setData(data: string): void;
    /**
     * @remarks
     * Registers a callback to be invoked whenever the observable's
     * value changes. Returns the callback, which can be passed to
     * unsubscribe to remove the listener.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * A function that receives the new string value each time the
     * observable changes.
     */
    subscribe(callback: (arg0: string) => void): (arg0: string) => void;
    /**
     * @remarks
     * Removes a previously registered listener from this
     * observable. Returns true if the listener was found and
     * removed, false if it was not found.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * The callback handle previously returned by subscribe.
     */
    unsubscribe(callback: (arg0: string) => void): boolean;
}

/**
 * An observable that holds a UIRawMessage value. Listeners are
 * notified whenever the value changes.
 */
export class ObservableUIRawMessage {
    /**
     * @remarks
     * Creates a new ObservableUIRawMessage with the provided
     * initial UIRawMessage value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The initial UIRawMessage value for this observable.
     * @param options
     * Optional configuration for the observable, such as whether
     * the value can be written by the client.
     */
    constructor(data: UIRawMessage, options?: ObservableOptions);
    /**
     * @remarks
     * Returns the current UIRawMessage value held by this
     * observable.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    getData(): UIRawMessage;
    /**
     * @remarks
     * Updates the UIRawMessage value held by this observable. If
     * the new value differs from the current value, all subscribed
     * listeners are notified with the new value.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param data
     * The new UIRawMessage value to set.
     */
    setData(data: UIRawMessage): void;
    /**
     * @remarks
     * Registers a callback to be invoked whenever the observable's
     * value changes. Returns the callback, which can be passed to
     * unsubscribe to remove the listener.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * A function that receives the new UIRawMessage value each
     * time the observable changes.
     */
    subscribe(callback: (arg0: UIRawMessage) => void): (arg0: UIRawMessage) => void;
    /**
     * @remarks
     * Removes a previously registered listener from this
     * observable. Returns true if the listener was found and
     * removed, false if it was not found.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * The callback handle previously returned by subscribe.
     */
    unsubscribe(callback: (arg0: UIRawMessage) => void): boolean;
}

export class UIManager {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    closeAllForms(player: Player): void;
}

/**
 * Options for configuring a button component.
 */
export interface ButtonOptions {
    /**
     * @remarks
     * When true or bound to a true ObservableBoolean, the button
     * is shown but cannot be pressed.
     *
     */
    disabled?: boolean | ObservableBoolean;
    /**
     * @remarks
     * Text shown in a tooltip when the player hovers over the
     * button.
     *
     */
    tooltip?: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the button
     * is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * Options for configuring a divider component in a CustomForm.
 */
export interface DividerOptions {
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the
     * divider is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * Represents a single item in a dropdown component.
 */
export interface DropdownItemData {
    /**
     * @remarks
     * Optional descriptive text shown around the dropdown when
     * this item is selected.
     *
     */
    description?: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * The text displayed for this item in the dropdown list.
     *
     */
    label: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * The numeric value associated with this dropdown item. This
     * is the value the bound ObservableNumber will be set to when
     * the player selects this item.
     *
     */
    value: number;
}

/**
 * Options for configuring a dropdown component.
 */
export interface DropdownOptions {
    /**
     * @remarks
     * Descriptive text shown around the dropdown to provide
     * additional context.
     *
     */
    description?: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * When true or bound to a true ObservableBoolean, the dropdown
     * is shown but cannot be changed.
     *
     */
    disabled?: boolean | ObservableBoolean;
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the
     * dropdown is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * @beta
 * Options for configuring an image component.
 */
export interface ImageOptions {
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the image
     * is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
    /**
     * @remarks
     * Sets the width of the image as a percentage of the width of
     * the form. 1 is the entire form width, 0.5 is half of the
     * form width. Greater than 1 or less than/equal to 0 will
     * become 1.
     *
     */
    width?: number | ObservableNumber;
}

/**
 * The result returned when an MessageBox is closed. Contains
 * the reason the message box was closed and the player's
 * button selection, if applicable.
 */
export interface MessageBoxResult {
    /**
     * @remarks
     * The reason the message box was closed.
     *
     */
    closeReason: DataDrivenScreenClosedReason;
    /**
     * @remarks
     * The index of the button the player selected. Not set if the
     * message box was closed without a button selection (e.g., the
     * player was busy or the server closed it).
     *
     */
    selection?: number;
}

/**
 * An interface that is passed into {@link
 * @minecraft/Server-ui.ModalFormData.dropdown} to provide
 * additional options for the dropdown creation.
 */
export interface ModalFormDataDropdownOptions {
    /**
     * @remarks
     * The default selected item index. It will be zero in case of
     * not setting this value.
     *
     */
    defaultValueIndex?: number;
    /**
     * @remarks
     * It will show an exclamation icon that will display a tooltip
     * if it is hovered.
     *
     */
    tooltip?: RawMessage | string;
}

/**
 * An interface that is passed into {@link
 * @minecraft/Server-ui.ModalFormData.slider} to provide
 * additional options for the slider creation.
 */
export interface ModalFormDataSliderOptions {
    /**
     * @remarks
     * The default value for the slider.
     *
     */
    defaultValue?: number;
    /**
     * @remarks
     * It will show an exclamation icon that will display a tooltip
     * if it is hovered.
     *
     */
    tooltip?: RawMessage | string;
    /**
     * @remarks
     * Defines the increment of values that the slider generates
     * when moved. It will be '1' in case of not providing this.
     *
     */
    valueStep?: number;
}

/**
 * An interface that is passed into {@link
 * @minecraft/Server-ui.ModalFormData.textField} to provide
 * additional options for the textfield creation.
 */
export interface ModalFormDataTextFieldOptions {
    /**
     * @remarks
     * The default value for the textfield.
     *
     */
    defaultValue?: RawMessage | string;
    /**
     * @remarks
     * It will show an exclamation icon that will display a tooltip
     * if it is hovered.
     *
     */
    tooltip?: RawMessage | string;
}

/**
 * An interface that is passed into {@link
 * @minecraft/Server-ui.ModalFormData.toggle} to provide
 * additional options for the toggle creation.
 */
export interface ModalFormDataToggleOptions {
    /**
     * @remarks
     * The default value for the toggle.
     *
     */
    defaultValue?: boolean;
    /**
     * @remarks
     * It will show an exclamation icon that will display a tooltip
     * if it is hovered.
     *
     */
    tooltip?: RawMessage | string;
}

/**
 * Configuration options for creating an Observable. Controls
 * how the observable value can be accessed and modified.
 */
export interface ObservableOptions {
    /**
     * @remarks
     * When true, allows the client to write to this observable's
     * value directly, enabling two-way data binding between the UI
     * and the observable.
     *
     */
    clientWritable: boolean;
}

/**
 * Options for configuring a slider component.
 */
export interface SliderOptions {
    /**
     * @remarks
     * Descriptive text shown around the slider to provide
     * additional context.
     *
     */
    description?: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * When true or bound to a true ObservableBoolean, the slider
     * is shown but cannot be moved.
     *
     */
    disabled?: boolean | ObservableBoolean;
    /**
     * @remarks
     * The increment amount between each slider step. Defaults to 1
     * if not specified.
     *
     */
    step?: number | ObservableNumber;
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the slider
     * is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * Options for configuring a spacer component.
 */
export interface SpacingOptions {
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the spacer
     * is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * Options for configuring a text field component.
 */
export interface TextFieldOptions {
    /**
     * @remarks
     * Descriptive text shown around the text field label to
     * provide additional context.
     *
     */
    description?: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * When true or bound to a true ObservableBoolean, the text
     * field is shown but cannot be edited.
     *
     */
    disabled?: boolean | ObservableBoolean;
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the text
     * field is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * Options for configuring a text component (label or header).
 */
export interface TextOptions {
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the text
     * component is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * Options for configuring a toggle component.
 */
export interface ToggleOptions {
    /**
     * @remarks
     * Descriptive text shown around the toggle to provide
     * additional context.
     *
     */
    description?: ObservableString | ObservableUIRawMessage | string | UIRawMessage;
    /**
     * @remarks
     * When true or bound to a true ObservableBoolean, the toggle
     * is shown but cannot be interacted with.
     *
     */
    disabled?: boolean | ObservableBoolean;
    /**
     * @remarks
     * When false or bound to a false ObservableBoolean, the toggle
     * is hidden.
     *
     */
    visible?: boolean | ObservableBoolean;
}

/**
 * A message that can be sent to the client. This is a subset
 * of the RawMessage type, and is used for UI messages.
 */
export interface UIRawMessage {
    /**
     * @remarks
     * Provides a raw-text equivalent of the current message.
     *
     */
    rawtext?: UIRawMessage[];
    /**
     * @remarks
     * Provides a string literal value to use.
     *
     */
    text?: string;
    /**
     * @remarks
     * Provides a localization string where, if the client has an
     * available resource in the players' language which matches
     * the localization string, will get translated on the client.
     *
     */
    translate?: string;
    /**
     * @remarks
     * Arguments for the localization string. Can be either an
     * array of strings or UIRawMessage containing an array of raw
     * text objects.
     *
     */
    with?: string[] | UIRawMessage;
}

/**
 * Thrown when a form is rejected. Contains the reason for the
 * rejection.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class FormRejectError extends Error {
    private constructor();
    /**
     * @remarks
     * The reason the form was rejected.
     *
     * @earlyExecution
     *
     */
    readonly reason: FormRejectReason;
}

/**
 * Thrown when a form visibility operation fails, such as
 * attempting to show a form that is already showing or
 * attempting to close a form that is not currently open.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class FormVisibilityError extends Error {
    private constructor();
    /**
     * @remarks
     * The identifier of the form that caused the visibility error.
     *
     * @earlyExecution
     *
     */
    readonly formId: string;
    /**
     * @remarks
     * The reason the form visibility operation failed.
     *
     * @earlyExecution
     *
     */
    readonly reason: FormVisibilityErrorReason;
}

/**
 * Thrown when attempting to interact with a form using an
 * invalid or unknown form identifier.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidFormError extends Error {
    private constructor();
    /**
     * @remarks
     * The identifier of the invalid form that was referenced.
     *
     * @earlyExecution
     *
     */
    readonly formId: string;
}

/**
 * Thrown when attempting to modify a form after it has already
 * been shown to a player. Form properties cannot be changed
 * while the form is active.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidFormModificationError extends Error {
    private constructor();
    /**
     * @remarks
     * The identifier of the form that was illegally modified after
     * being shown.
     *
     * @earlyExecution
     *
     */
    readonly formId: string;
}

/**
 * Thrown when an observable value is expected to be writable,
 * but it is not.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidObservableError extends Error {
    private constructor();
}

/**
 * Thrown when a form operation fails because the target player
 * has left the game.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerLeftError extends Error {
    private constructor();
    /**
     * @remarks
     * The identifier of the form that was being shown when the
     * player left the game.
     *
     * @earlyExecution
     *
     */
    readonly formId: string;
}

export const uiManager: UIManager;

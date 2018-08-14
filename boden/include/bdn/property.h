#ifndef BDN_property_H_
#define BDN_property_H_

#include <bdn/PlainPropertyReadAccessor.h>
#include <bdn/PropertyNotifier.h>

namespace bdn
{
    
/** \def BDN_FINALIZE_CUSTOM_PROPERTY_WITH_CUSTOM_ACCESS( valueType, readAccess, name, writeAccess, setterName, ...)
     
    Finalizes the definition of a custom property with custom access permissions.
     
    This macro works the same way as \ref BDN_FINALIZE_CUSTOM_PROPERTY, except that one can specify
    the access specifier (public, protected, private) for read and write access.
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param readAccess access specifier for read access. This is one of the normal C++ access specifiers:
      public, protected, private.
    \param name the name of the property
    \param writeAccess access specifier for write access. This is one of the normal C++ access specifiers:
        public, protected, private.
    \param setterName the name of the property's setter function
    \param ... The last parameter is optional. It can either be omitted or it can be a combination of
    override and/or final. These work the same as the override and final specifiers on virtual functions.
    override must be specified when the property provides the implementation for an abstract property inherited
    from a base class or interface (see \ref BDN_ABSTRACT_PROPERTY).
    final can optionally be specified to "lock down" the property and prevent derived classes from changing
    its behavior.
    */
#define BDN_FINALIZE_CUSTOM_PROPERTY_WITH_CUSTOM_ACCESS( valueType, readAccess, name, writeAccess, setterName, ...) \
readAccess: \
    using PropertyValueType_##name = valueType; \
protected: \
    virtual void _propertiesCannotBeRedeclaredInDerivedClass_OverrideGetterSetterToCustomize_##name() final {} \
public:
    

/** \def BDN_FINALIZE_CUSTOM_READ_ONLY_PROPERTY( valueType, name, ...)
     
     Finalizes the definition of a custom read only property.
 
     This macro works the same way as \ref BDN_FINALIZE_CUSTOM_PROPERTY, except
     that the property has no setter function (so it is read-only).
 */
#define BDN_FINALIZE_CUSTOM_READ_ONLY_PROPERTY( valueType, name, ...) \
    using PropertyValueType_##name = valueType; \
protected: \
    virtual void _propertiesCannotBeRedeclaredInDerivedClass_OverrideGetterSetterToCustomize_##name() final {}  \
public:
    

/** \def BDN_FINALIZE_CUSTOM_PROPERTY( valueType, name, setterName, ...)
 
    Finalizes the definition of a custom property. A custom property is
    one in which the getter function, setter function and the function returning
    the "changed" notifier have been written manually, instead of being generated
    by one of the property macros.
 
    BDN_FINALIZE_CUSTOM_PROPERTY assumes that these three functions already exist
    and finishes up the property definition.
 
    The three functions must conform to the following:
 
    - The getter function must take no parameters and return the property value
      (either as a copy or const reference).
 
      The getter function must be const and have the same name as the property
      (e.g. myProperty() ).
 
    - The changed function must return a reference to an IPropertyNotifier<VALUE_TYPE>
      object that fires when the property value changes (VALUE_TYPE being the property's value type).
 
      The changed function must be const and have the name of the property, followed by "Changed"
      (e.g. myPropertyChanged() ).
 
    - The setter function must take a single parameter (either a copy or const reference
      to the new property value).
 
      The setter function is responsible for calling notify() on the changed notifier when
      the property value changes. If the setter is called with a new value that equals the current
      value then it **must not** fire the changed notifier (i.e. setting the same value should have
      no effect).
 
      The setter can have an arbitrarily chosen name, but the convention is that it is "set" followed by the
      property name in CamelCase (e.g. setMyProperty() ).
 
 
    If you want to create a read-only property then you can use
    \ref BDN_FINALIZE_CUSTOM_READ_ONLY_PROPERTY instead and omit the setter function.
 
    Example:
 
    \code
 
    class Length
    {
    public:
 
        // this is the getter. It must be const and return the value
        // (as a copy or const reference).
        // The getter must have the same name as the property and must be
        // a const function.
        double centimeters() const
        {
            return _meters * 100;
        }
 
        // The setter takes the new value as its parameter.
        // It can have any name, but the convention is to name it setXyz.
        void setCentimeters( double newValue )
        {
            // first we must check if the new value is different from the current one.
            // That is required, because we must not fire the changed notifier
            // when the new value is the same.
            if( newValue != centimeters() )
            {
                // then we store the value
                _meters = newValue / 100;
 
                // and then we fire the changed notifier, since the value has changed.
                BDN_NOTIFY_PROPERTY_CHANGED( *this, centimeters );
            }
        }
 
        // The changed function must return a reference to the notifier that fires
        // when the property value changed. Here we just use the default implementation.
        BDN_DEFAULT_PROPERTY_CHANGED( double, centimeters );
 
        // the last step is to add the "finalize" macro. This sets up some additional
        // internal helper items that are needed for the property to work correctly.
        BDN_FINALIZE_CUSTOM_PROPERTY( double, centimeters, setCentimeters );
 
    };
 
    \endcode
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param name the name of the property
    \param setterName name of the property's setter function
    \param ... The third parameter is optional. It can either be omitted or it can be a combination of
        override and/or final. These work the same as the override and final specifiers on virtual functions.
        override must be specified when the property provides the implementation for an abstract property inherited
        from a base class or interface (see \ref BDN_ABSTRACT_PROPERTY).
        final can optionally be specified to "lock down" the property and prevent derived classes from changing
        its behavior.
    */
#define BDN_FINALIZE_CUSTOM_PROPERTY( valueType, name, setterName, ...) \
    BDN_FINALIZE_CUSTOM_READ_ONLY_PROPERTY( valueType, name);
    

    

    
/** Provides a default implemenation for the "changed" function of a custom property.
 
    This can be used when a custom property is defined, but no special behavior is needed for
    the changed notifier. See \ref BDN_FINALIZE_CUSTOM_PROPERTY for more information.
 
    The macro adds a pointer bdn::PropertyNotifier object as a member variable and generates the "changed" function so
    that it returns a reference to the notifier. In the current implementation, the notifier object is allocated
    dynamically on first use - so it adds only minimal overhead when not used.
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param propertyName the name of the property
    \param ... The last parameter is optional. It can either be omitted or it can be a combination of
        override and/or final. These work the same as the override and final specifiers on virtual functions.
        override must be specified when the property provides the implementation for an abstract property inherited
        from a base class or interface (see \ref BDN_ABSTRACT_PROPERTY).
        final can optionally be specified to "lock down" the property and prevent derived classes from changing
        its behavior.
    */
#define BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION( valueType, propertyName, ... ) \
    virtual bdn::IPropertyNotifier<valueType>& propertyName##Changed() const __VA_ARGS__ { \
        if(_pPropertyChanged_##propertyName == nullptr ) \
           _pPropertyChanged_##propertyName = bdn::newObj< bdn::PropertyNotifier<valueType> >(); \
        return *_pPropertyChanged_##propertyName; \
    } \
    private: \
    mutable bdn::P< bdn::PropertyNotifier<valueType> > _pPropertyChanged_##propertyName; \
    public:
    
    
    
/** \def BDN_PROPERTY_WITH_CUSTOM_ACCESS( valueType, readAccess, name, writeAccess, setterName, ...)
 
    Like \ref BDN_PROPERTY, except that one can specify the access permissions for read and write access
    (public, protected, private).
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param readAccess access specifier for read access. This is one of the normal C++ access specifiers:
      public, protected, private. It defines the access for the getter function, the function that returns
      the changed notifier and the PropertyValueType_NAME typedef.
    \param name the name of the property
    \param writeAccess access specifier for write access. This is one of the normal C++ access specifiers:
        public, protected, private. It defines the access for the setter function.
    \param setterName the name of the property's setter function
    \param ... The last parameter is optional. It can either be omitted or it can be a combination of
    override and/or final. These work the same as the override and final specifiers on virtual functions.
    override must be specified when the property provides the implementation for an abstract property inherited
    from a base class or interface (see \ref BDN_ABSTRACT_PROPERTY).
    final can optionally be specified to "lock down" the property and prevent derived classes from changing
    its behavior.
    */
#define BDN_PROPERTY_WITH_CUSTOM_ACCESS( valueType, readAccess, name, writeAccess, setterName, ... ) \
    readAccess: \
    virtual valueType name() const __VA_ARGS__ \
    { \
        return _propertyValue_##name; \
    } \
    writeAccess: \
    virtual void setterName(const valueType& value) __VA_ARGS__ \
    { \
        if( _propertyValue_##name != value ) \
        { \
            _propertyValue_##name = value; \
            BDN_NOTIFY_PROPERTY_CHANGED( *this, name ); \
        } \
    } \
    private: \
    valueType _propertyValue_##name{}; \
    readAccess: \
    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION( valueType, name ); \
    BDN_FINALIZE_CUSTOM_PROPERTY_WITH_CUSTOM_ACCESS( valueType, readAccess, name, writeAccess, setterName, __VA_ARGS__);

    
/** \def BDN_PROPERTY( valueType, name, setterName, ... )
 
    Defines a normal property, with a default getter and setter.
 
    In Boden, properties are simply a set of functions that an owner object provides.
    They handle access to the underlying internal property value and enable change notifications
    and property binding (see \ref BDN_BIND_TO_PROPERTY and \ref BDN_BIND_TO_PROPERTY_WITH_FILTER).
 
    More information: \ref properties.md "Properties"
 
    BDN_PROPERTY provides a default implementation for the property access functions.
    The internal value is stored in a private member variable. The access functions are not thread safe.
 
    Note that BDN_PROPERTY can be replaced later with any of the other property definition macros
    without changing the public interface of the class. This means that when custom getter or setter
    functionality is desired in the future then BDN_PROPERTY can be replaced with one of the
    other property macros without breaking the public interface.
 
    Note that derived classes can fully override the behavior of the property, unless you specify
    "final" as the fourth parameter (see example).
 
    BDN_PROPERTY is desigend to create a public property (using the C++ access specifier "public").
    Use \ref BDN_PROPERTY_WITH_CUSTOM_ACCESS to create properties with different access specifiers.
    Also note that BDN_PROPERTY ends with the public access specifier active - so any following code will be
    public by default.
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param name the name of the property
    \param setterName the name of the property's setter function
    \param ... The fourth parameter is optional. It can either be omitted or it can be a combination of
        override and/or final. These work the same as the override and final specifiers on virtual functions.
        override must be specified when the property provides the implementation for an abstract property inherited
        from a base class or interface (see \ref BDN_ABSTRACT_PROPERTY).
        final can optionally be specified to "lock down" the property and prevent derived classes from changing
        its behavior.
    */
#define BDN_PROPERTY( valueType, name, setterName, ... ) \
    virtual valueType name() const __VA_ARGS__ \
    { \
        return _propertyValue_##name; \
    } \
    virtual void setterName(const valueType& value) __VA_ARGS__ \
    { \
        if( _propertyValue_##name != value ) \
        { \
            _propertyValue_##name = value; \
            BDN_NOTIFY_PROPERTY_CHANGED( *this, name ); \
        } \
    } \
    private: \
    valueType _propertyValue_##name{}; \
    public: \
    BDN_PROPERTY_CHANGED_DEFAULT_IMPLEMENTATION( valueType, name ); \
    BDN_FINALIZE_CUSTOM_PROPERTY( valueType, name, setterName, __VA_ARGS__);


   
/** \def BDN_ABSTRACT_PROPERTY( valueType, name, setterName )
 
    Defines an abstract property - i.e. a property whose implementation must be
    provided by a derived class. They work the same way as abstract / pure virtual functions.
 
    The implementations for an abstract property can be provided with \ref BDN_PROPERTY.
 
    Example:
 
    \code
 
    class IAddress
    {
    public:
 
        // define an abstract string property called "street", with the setter function
        // being called "setStreet".
        BDN_ABSTRACT_PROPERTY( String, street, setStreet );
    };
 
    class Address : BDN_IMPLEMENTS IAddress
    {
    public:
 
        // define an implementation for the "street" property from IAddress.
        // Note that, like with virtual functions, we have to specify "override" at the end
        // because we are overriding an abstract property.
        BDN_PROPERTY( String, street, setStreet, override );
    };
 
    IAddress* pAddress = ...
 
    // we can now use the street property via an IAddress pointer.
    pAddress->setStreet("My Street 42");
 
    \endcode
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param name the name of the property
    \param setterName the name of the property's setter function
 */
#define BDN_ABSTRACT_PROPERTY( valueType, name, setterName ) \
    virtual valueType name() const=0; \
    virtual void setterName(const valueType& value)=0; \
    virtual IPropertyNotifier<valueType> & name##Changed() const=0; \
    using PropertyValueType_##name = valueType;

   
/** \def BDN_NOTIFY_PROPERTY_CHANGED( owner, name )
 
    Indicates that the value of a property has changed and calls "notify" on the
    changed notifier of the property.
 
    \param owner the owner of the property (a reference or object, not a pointer)
    \param name the name of the property. This must be the same name that was also
      specified when the property was defined (see \ref BDN_PROPERTY).
    */
#define BDN_NOTIFY_PROPERTY_CHANGED( owner, name ) \
    (owner).name##Changed().notify( BDN_PROPERTY_READ_ACCESSOR( owner, name ) );


/** Provides a way to disambiguate a property when the same property is inherited from
    multiple base classes / interfaces.
 
    When that happens then the compiler will not know which inherited version to use,
    so a compiler error will occur when the property is accessed.
 
    BDN_DISAMBIGUATE_PROPERTY can be used to solve these cases - it explicitly selects
    which one of the inherited versions should be used.
 
    \param valueType the type of the internal property value. This must be a valid C++ type or class name.
    \param name the name of the property
    \param setterName the name of the property's setter function
    \param baseClassToUsePropertyFrom the name of the base class, whose version of the property should be used.
        When a property is inherited from both a normal base class and from an interface, then the normal base
        class should be specified here.
 */
#define BDN_DISAMBIGUATE_PROPERTY( valueType, name, setterName, baseClassToUsePropertyFrom ) \
    virtual valueType name() const override { \
        return baseClassToUsePropertyFrom::name(); \
    } \
    virtual void setterName(const valueType& value) override { \
        baseClassToUsePropertyFrom::setterName(value); \
    } \
    virtual bdn::IPropertyNotifier<valueType>& name##Changed() const override { \
        return baseClassToUsePropertyFrom::name##Changed(); \
    } \
    using PropertyValueType_##name = valueType;


template<typename VALUE_TYPE, typename OWNER_TYPE, typename SETTER_METHOD_TYPE>
std::function< void(const VALUE_TYPE&) > _makePropertySubscriber( OWNER_TYPE* pOwner, SETTER_METHOD_TYPE&& setterMethod )
{
    WeakP<OWNER_TYPE> weakOwner(pOwner);
    
    return [weakOwner, setterMethod]( const VALUE_TYPE& value ) {
        P<OWNER_TYPE> strongOwner = weakOwner.toStrong();
        if(strongOwner==nullptr)
            throw DanglingFunctionError();
        
        ((*strongOwner).*setterMethod)( value );
    };
}
    
    
template<typename VALUE_TYPE, typename OWNER_TYPE, typename SETTER_METHOD_TYPE, typename FILTER_FUNC_TYPE>
std::function< void(const VALUE_TYPE&) > _makePropertySubscriberWithFilter( OWNER_TYPE* pOwner, SETTER_METHOD_TYPE&& setterMethod, FILTER_FUNC_TYPE&& filterFunc )
{
    WeakP<OWNER_TYPE> weakOwner(pOwner);
    
    return [weakOwner, setterMethod, filterFunc]( const VALUE_TYPE& value ) {
        P<OWNER_TYPE> strongOwner = weakOwner.toStrong();
        if(strongOwner==nullptr)
            throw DanglingFunctionError();
        
        ((*strongOwner).*setterMethod)( filterFunc( value ) );
    };
}
    
/** \def BDN_BIND_TO_PROPERTY( receiverOwner, receiverSetterName, senderOwner, senderPropName )
 
    Binds a property to another property.
 
    Property binding enables one to connect two properties together, so that when one changes, then
    the other one will automatically be set to the same value.
 
    The receiver property is the one that is automatically updated. The sender property is the one
    that is monitored for changes.
 
    Initially, when the binding is made then the receiver property is automatically set to the current value
    of the sender. So they are at the same value directly after the binding is complete.
 
    Note that the binding is only unidirectional. So the sender is NOT automatically updated
    when the receiver changes. When you want both properties to be fully synchronized to the same value,
    no matter which one changes, then you either need to make two bindings or use \ref BDN_BIND_PROPERTIES.
 
    There is also a way to bind two properties with a value filter in between (i.e.
    set the receiver to a filtered or modified version of the source value). See \ref BDN_BIND_TO_PROPERTY_WITH_FILTER.
 
    \param receiverOwner the owner of the receiver property. This must be the owner object or a reference to it
        (not a pointer).
    \param receiverSetterName the name of the receiver property's setter function.
    \param senderOwner the owner of the sender property.  This must be the owner object or a reference to it
        (not a pointer).
    \param senderPropName the name of the sender property.
 */
#define BDN_BIND_TO_PROPERTY( receiverOwner, receiverSetterName, senderOwner, senderPropName ) \
    (senderOwner).senderPropName##Changed().subscribe( \
        bdn::_makePropertySubscriber< typename std::remove_reference<decltype(senderOwner)>::type :: PropertyValueType_##senderPropName> ( \
            &(receiverOwner), \
            & std::remove_reference<decltype(receiverOwner)>::type :: receiverSetterName \
        ) \
    ); \
    (receiverOwner).receiverSetterName( (senderOwner).senderPropName() );
    

template<typename VALUE_TYPE, typename OWNER_TYPE, typename SETTER_METHOD_TYPE, typename FILTER_FUNC_TYPE>
std::function< void(const VALUE_TYPE&) > makePropertySubscriberWithFilter( OWNER_TYPE* pOwner, SETTER_METHOD_TYPE&& setterMethod, FILTER_FUNC_TYPE&& filterFunc)
{
    return [pOwner, setterMethod, filterFunc]( const VALUE_TYPE& value ) {
        (pOwner->*setterMethod)( filterFunc(value) );
    };
}

/** \def BDN_BIND_TO_PROPERTY_WITH_FILTER( receiverOwner, receiverSetterName, senderOwner, senderPropName, filterFunc )
 
    Binds a property to another property with a filter function in between.
 
    Please read the documentation of \ref BDN_BIND_TO_PROPERTY first, as
    BDN_BIND_TO_PROPERTY_WITH_FILTER is very similar to that. The only difference is that
    the receiver property is not set to exactly the same value as the sender property.
    Instead, the new value is passed to a filter function, which generates the value for the receiver property.
 
    The filter function is often a lambda function.
 
    Note that when the binding is initially created, the receiver property is automatically set to the current value of the sender.
 
    Example:
 
    \code
 
    // In this example we create a view model - i.e. an object that prepares the input data that is shown
    // in the UI of the program.
    class MyViewModel
    {
    public:
 
        // progressText is a String property that shows the progress of the current operation.
        BDN_PROPERTY( String, progressText, setProgressText );
    };
 
    // BackgroundOperation represents some longer operation that is done in the background.
    class MyBackgroundOperation
    {
    public:
 
        // progressPercent is a double property that contains the progress percentage.
        BDN_PROPERTY( double, progressPercent, setProgressPercent );
    };
 
    // In this example we have one instance of our view model and of some background operation
    MyViewModel             viewModel = ...;
    MyBackgroundOperation   backgroundOp = ...;
 
    // we now want to connect the progress text of the view model to the progress percentage
    // property of the background operation. Obviously this does not work with a normal connection,
    // since one is a number and the other one is a text. But we can provide a filter function
    // that does the conversion.
 
    BDN_BIND_TO_PROPERTY_WITH_FILTER( viewModel, setProgressText, backgroundOp, progressPercent,
        [](double percent) -> String
        {
            // generate our text
            return std::to_string(percent)+" % done".
        } );
 
    // now when we set the progress of the background operation the progress text is updated
    // automatically.
    backgroundOp.setProgressPercent( 42 );
    String text = viewModel.progressText();     // this will now be the text "42 % done"
 
    \endcode
 
    \param receiverOwner the owner of the receiver property. This must be the owner object or a reference to it
        (not a pointer).
    \param receiverSetterName the name of the receiver property's setter function.
    \param senderOwner the owner of the sender property.  This must be the owner object or a reference to it
        (not a pointer).
    \param senderPropName the name of the sender property.
    \param filterFunc the filter function to use.
 */
#define BDN_BIND_TO_PROPERTY_WITH_FILTER( receiverOwner, receiverSetterName, senderOwner, senderPropName, filterFunc ) \
    (senderOwner).senderPropName##Changed().subscribe( \
        bdn::_makePropertySubscriberWithFilter< typename std::remove_reference<decltype(senderOwner)>::type :: PropertyValueType_##senderPropName> ( \
            &(receiverOwner), \
            & std::remove_reference<decltype(receiverOwner)>::type :: receiverSetterName, \
            filterFunc \
        ) \
    ); \
    (receiverOwner).receiverSetterName( (filterFunc)( (senderOwner).senderPropName() ) );



/** Binds two properties to each other, so that when either one changes then the other
    one is automatically set to the same value (bidirectional binding).

    Note that in contrast to \ref BDN_BIND_TO_PROPERTY, the binding works in both directions.
 
    Also note that when the binding is initially created, then the first property is set to the current value
    of the second property. So after BDN_BIND_PROPERTIES finishes, both properties will have the same value. 
 
    \param receiverOwner the owner of the receiver property. This must be the owner object or a reference to it
        (not a pointer).
    \param receiverSetterName the name of the receiver property's setter function.
    \param senderOwner the owner of the sender property.  This must be the owner object or a reference to it
        (not a pointer).
    \param senderPropName the name of the sender property.
    \param filterFunc the filter function to use.
 */
#define BDN_BIND_PROPERTIES( ownerA, getterNameA, setterNameA, ownerB, getterNameB, setterNameB ) \
    BDN_BIND_TO_PROPERTY( ownerA, setterNameA, ownerB, getterNameB ); \
    BDN_BIND_TO_PROPERTY( ownerB, setterNameB, ownerA, getterNameA );
    
    

    
}

#endif
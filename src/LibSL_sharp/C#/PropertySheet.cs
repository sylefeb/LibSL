// SL: 2015

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Windows.Forms;

/*
 * Fakes a ICustomTypeDescriptor to use with the PropertyGrid control 
 * see also
 * - https://msdn.microsoft.com/en-us/library/aa302326.aspx
 * - http://www.codeproject.com/Articles/3397/Bending-the-NET-PropertyGrid-to-Your-Will
 * - http://www.codeproject.com/Articles/415070/Dynamic-Type-Description-Framework-for-PropertyGri
 */

namespace LibSL_sharp
{
  // Handler called whenever a custom property is changed
  public delegate void OnCustomPropertyChangeHandler(object sender, CustomPropertyEventArgs e);

  public class CustomPropertyEventArgs : EventArgs
  {
    private CustomProperty m_Property;
    private object m_Value;

    public CustomPropertyEventArgs(CustomProperty p, object v)
    {
      m_Property = p;
      m_Value = v;
    }
    public CustomProperty Property
    {
      get { return m_Property; } 
    }
    public object Value
    {
      get 
      { 
        return m_Value; 
      }
      set 
      {
        m_Value = value; 
      }
    }
  }

  public class CustomProperty
  {
    // Property level handlers
    public event OnCustomPropertyChangeHandler OnGetValue;
    public event OnCustomPropertyChangeHandler OnSetValue;
    // Information about the custom property
    public string m_Name = null;
    public Type m_Type = null;
    public string m_Category = null;
    public Type m_Converter = null;
    public Type m_Editor = null;
    public Object m_UserData = null;
    // Creating a property (simple)
    public CustomProperty(string name, Type type, string category)
    {
      m_Name = name;
      m_Type = type;
      m_Category = category;
    }
    // Creating a property (advanced: editor and converter)
    public CustomProperty(string name, Type type, string category, Type editor, Type converter)
      : this(name, type, category)
    {
      m_Editor = editor;
      m_Converter = converter;
    }
    public virtual void TriggerOnGetValue(CustomPropertyEventArgs e)
    {
      if (OnGetValue != null)
      {
        OnGetValue(this, e);
      }
    }
    public virtual void TriggerOnSetValue(CustomPropertyEventArgs e)
    {
      if (OnSetValue != null)
      {
        OnSetValue(this, e);
      }
    }
  }

  // Inherits from PropertyDescriptor to describe the content of our custom property
  public class CustomPropertyDescriptor : PropertyDescriptor
  {
    public CustomPropertyDescriptor(CustomProperty item, PropertySheet owner, string name, Attribute[] attributes)
      : base(name, attributes)
    {
      m_Owner = owner;
      m_Property = item;
    }

    private PropertySheet m_Owner;
    private CustomProperty m_Property;

    public override Type ComponentType
    {
      get { return m_Property.GetType(); }
    }
    public override bool IsReadOnly
    {
      get { return false; }
    }
    public override Type PropertyType
    {
      get { return m_Property.m_Type; }
    }
    public override bool CanResetValue(object component)
    {
      return false;
    }
    public override object GetValue(object component)
    {
      CustomPropertyEventArgs e = new CustomPropertyEventArgs(m_Property, null);
      m_Owner.TriggerOnGetValue(e);
      m_Property.TriggerOnGetValue(e);
      return e.Value;
    }
    public override void ResetValue(object component)
    {
      SetValue(component, null);
    }
    public override void SetValue(object component, object value)
    {
      CustomPropertyEventArgs e = new CustomPropertyEventArgs(m_Property, value);
      m_Owner.TriggerOnSetValue(e);
      m_Property.TriggerOnSetValue(e);
    }
    public override bool ShouldSerializeValue(object component)
    {
      return true;
    }
  }

  public class PropertySheet : ICustomTypeDescriptor
  {
    public PropertySheet()
    {
    }

    private List<CustomProperty> m_Properties = new List<CustomProperty>();
    public event OnCustomPropertyChangeHandler OnGetValue;
    public event OnCustomPropertyChangeHandler OnSetValue;

    // A UserData is always useful!
    private Object m_UserData = null;
    public Object UserData { get { return m_UserData; } set { m_UserData = value; } }

    // Add a property to the sheet
    public void AddProperty(CustomProperty prop)
    {
      m_Properties.Add(prop);
    }

    public virtual void TriggerOnGetValue(CustomPropertyEventArgs e)
    {
      if (OnGetValue != null)
      {
        OnGetValue(this, e);
      }
    }
    public virtual void TriggerOnSetValue(CustomPropertyEventArgs e)
    {
      if (OnSetValue != null)
      {
        OnSetValue(this, e);
      }
    }
    // --- implementation of ICustomTypeDescriptor ---
    PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties(Attribute[] attributes)
    {
      // Generate fake type attributes
      List<CustomPropertyDescriptor> descriptors = new List<CustomPropertyDescriptor>();
      foreach (CustomProperty property in m_Properties)
      {
        List<Attribute> all_attributes = new List<Attribute>();
        if (property.m_Category != null)
        {
          all_attributes.Add(new CategoryAttribute(property.m_Category));
        }
        if (property.m_Editor != null)
        {
          all_attributes.Add(new EditorAttribute(property.m_Editor, typeof(UITypeEditor)));
        }
        if (property.m_Converter != null)
        {
          all_attributes.Add(new TypeConverterAttribute(property.m_Converter));
        }
        CustomPropertyDescriptor pd = new CustomPropertyDescriptor(property, this, property.m_Name, all_attributes.ToArray());
        descriptors.Add(pd);
      }
      return new PropertyDescriptorCollection(descriptors.ToArray());
    }
    AttributeCollection ICustomTypeDescriptor.GetAttributes()
    {
      return TypeDescriptor.GetAttributes(this, true);
    }
    string ICustomTypeDescriptor.GetClassName()
    {
      return TypeDescriptor.GetClassName(this, true);
    }
    string ICustomTypeDescriptor.GetComponentName()
    {
      return TypeDescriptor.GetComponentName(this, true);
    }
    TypeConverter ICustomTypeDescriptor.GetConverter()
    {
      return TypeDescriptor.GetConverter(this, true);
    }
    EventDescriptor ICustomTypeDescriptor.GetDefaultEvent()
    {
      return TypeDescriptor.GetDefaultEvent(this, true);
    }
    PropertyDescriptor ICustomTypeDescriptor.GetDefaultProperty()
    {
      return null;
    }
    object ICustomTypeDescriptor.GetEditor(Type editorBaseType)
    {
      return TypeDescriptor.GetEditor(this, editorBaseType, true);
    }
    EventDescriptorCollection ICustomTypeDescriptor.GetEvents()
    {
      return TypeDescriptor.GetEvents(this, true);
    }
    EventDescriptorCollection ICustomTypeDescriptor.GetEvents(Attribute[] attributes)
    {
      return TypeDescriptor.GetEvents(this, attributes, true);
    }
    PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties()
    {
      return ((ICustomTypeDescriptor)this).GetProperties(new Attribute[0]);
    }
    object ICustomTypeDescriptor.GetPropertyOwner(PropertyDescriptor pd)
    {
      return this;
    }
  }

}

#ifndef BDN_MAC_ButtonCore_HH_
#define BDN_MAC_ButtonCore_HH_

#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

#import <bdn/mac/ButtonCoreBase.hh>

namespace bdn
{
namespace mac
{

class ButtonCore : public ButtonCoreBase, BDN_IMPLEMENTS IButtonCore
{
private:
    static NSButton* _createNsButton(Button* pOuterButton)
    {
        NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        
        [button setButtonType:NSMomentaryLightButton];
        [button setBezelStyle:NSRoundedBezelStyle];
    
        return button;
    }
    

public:
    ButtonCore(Button* pOuterButton);
        
    void setLabel(const String& label) override
    {
        ButtonCoreBase::setLabel(label);
    }
    
    void setBounds(const Rect& bounds) override
    {
        ChildViewCore::setBounds(bounds);
        _updateBezelStyle();
    }

    
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        ChildViewCore::setPadding(padding);
        
        // we may need to update the bezel style.
    }
    
    
    Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const override
    {
        // the bezel style influences the fitting size. To get consistent values here we have to
        // ensure that we use the same bezel style each time we calculate the size.
        
        NSBezelStyle bezelStyle = _nsButton.bezelStyle;
        if(bezelStyle != NSRoundedBezelStyle)
            _nsButton.bezelStyle = NSRoundedBezelStyle;
        
        Size size = ButtonCoreBase::calcPreferredSize(availableWidth, availableHeight);
        
        if(bezelStyle != NSRoundedBezelStyle)
            _nsButton.bezelStyle = bezelStyle;
        
        return size;
    }
    
    void generateClick();
    
protected:
    Margin getPaddingIncludedInFittingSize() const override
    {
        // the button automatically includes some padding in the fitting size.
        // This is rougly this:
        
        return uiMarginToPixelMargin( UiMargin(UiLength::sem, 0.5, 1) );
    }
    
private:
    void _updateBezelStyle();    
    
    NSObject*   _clickManager;
    
    NSBezelStyle _currBezelStyle;
    int          _heightWithRoundedBezelStyle;
};

}
}


#endif
/**
 * Products list route (/products)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderProductsRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Products", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("Product List - Click on a product to view details", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Example product links
    VStack productList;
    productList.setSpacing(10.0);
    
    Link product1;
    product1.create("/products/1", "Product 1", 0, 0, 150, 30);
    productList.addChild(product1);
    
    Link product2;
    product2.create("/products/2", "Product 2", 0, 0, 150, 30);
    productList.addChild(product2);
    
    Link product3;
    product3.create("/products/3", "Product 3", 0, 0, 150, 30);
    productList.addChild(product3);
    
    content.addChild(productList);
    
    window.setContent(content);
}

REGISTER_ROUTE("/products", renderProductsRoute);

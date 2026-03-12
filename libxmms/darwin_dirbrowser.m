#import <Cocoa/Cocoa.h>

typedef void (*DirSelectHandler)(char *path);

@interface DirNode : NSObject
@property(nonatomic,strong) NSString *path;
@property(nonatomic,strong) NSMutableArray *children;
@property(nonatomic,assign) BOOL scanned;
@end

@implementation DirNode
@end


@interface DirBrowserController : NSWindowController <NSOutlineViewDelegate, NSOutlineViewDataSource>
{
    NSOutlineView *outline;
    DirNode *rootNode;
    DirSelectHandler handler;
}
@end


@implementation DirBrowserController

- (instancetype)initWithTitle:(NSString*)title
                  currentPath:(NSString*)path
                      handler:(DirSelectHandler)cb
{
    NSWindow *window =
        [[NSWindow alloc] initWithContentRect:NSMakeRect(200,200,400,400)
                                     styleMask:(NSWindowStyleMaskTitled |
                                                NSWindowStyleMaskClosable |
                                                NSWindowStyleMaskResizable)
                                       backing:NSBackingStoreBuffered
                                         defer:NO];

    self = [super initWithWindow:window];
    if(self)
    {
        handler = cb;
        window.title = title;

        NSScrollView *scroll = [[NSScrollView alloc] initWithFrame:window.contentView.bounds];
        scroll.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        outline = [[NSOutlineView alloc] initWithFrame:scroll.bounds];

        NSTableColumn *column =
            [[NSTableColumn alloc] initWithIdentifier:@"dir"];
        column.title = @"Directories";

        [outline addTableColumn:column];
        outline.outlineTableColumn = column;
        outline.delegate = self;
        outline.dataSource = self;

        scroll.documentView = outline;
        [window.contentView addSubview:scroll];

        rootNode = [self createNode:@"/"];
    }

    return self;
}

#pragma mark - Tree Model

- (DirNode*)createNode:(NSString*)path
{
    DirNode *node = [DirNode new];
    node.path = path;
    node.children = [NSMutableArray array];
    node.scanned = NO;
    return node;
}

- (void)scanNode:(DirNode*)node
{
    if(node.scanned) return;

    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *contents =
        [fm contentsOfDirectoryAtPath:node.path error:nil];

    for(NSString *entry in contents)
    {
        if([entry hasPrefix:@"."]) continue;

        NSString *full =
            [node.path stringByAppendingPathComponent:entry];

        BOOL isDir;
        if([fm fileExistsAtPath:full isDirectory:&isDir] && isDir)
        {
            DirNode *child = [self createNode:full];
            [node.children addObject:child];
        }
    }

    node.scanned = YES;
}

#pragma mark - OutlineView DataSource

- (NSInteger)outlineView:(NSOutlineView *)outlineView
 numberOfChildrenOfItem:(id)item
{
    DirNode *node = item ?: rootNode;
    [self scanNode:node];
    return node.children.count;
}

- (id)outlineView:(NSOutlineView *)outlineView
           child:(NSInteger)index
          ofItem:(id)item
{
    DirNode *node = item ?: rootNode;
    return node.children[index];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
   isItemExpandable:(id)item
{
    return YES;
}

#pragma mark - Cell Display

- (NSView*)outlineView:(NSOutlineView*)ov
    viewForTableColumn:(NSTableColumn*)col
                  item:(id)item
{
    NSTextField *cell =
        [ov makeViewWithIdentifier:@"cell" owner:self];

    if(!cell)
    {
        cell = [[NSTextField alloc] init];
        cell.identifier = @"cell";
        cell.bezeled = NO;
        cell.drawsBackground = NO;
        cell.editable = NO;
    }

    DirNode *node = item;
    cell.stringValue = node.path.lastPathComponent;

    return cell;
}

#pragma mark - Selection

- (void)outlineViewSelectionDidChange:(NSNotification *)note
{
    NSInteger row = outline.selectedRow;
    if(row < 0) return;

    DirNode *node = [outline itemAtRow:row];

    if(handler)
        handler((char*)node.path.UTF8String);
}

@end

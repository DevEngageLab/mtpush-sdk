// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "UAPush",
    platforms: [.iOS(.v10)],
    products: [
        // Products define the executables and libraries a package produces, and make them visible to other packages.
        .library(
            name: "MTPush",
            targets: ["MTPushWrapper"]),
        .library(
            name: "MTMA",
            targets: ["MTMAWrapper"]),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        // .package(url: /* package url */, from: "1.0.0"),
    ],
    targets: [
        // Targets are the basic building blocks of a package. A target can define a module or a test suite.
        // Targets can depend on other targets in this package, and on products in packages this package depends on.
        .target(
            name: "MTPushWrapper",
            dependencies: ["MTPush"], 
            path: "MTPushWrapper",
            linkerSettings: [ 
                .linkedLibrary("z"),
                .linkedLibrary("resolv"),
                .linkedFramework("UIKit"),
                .linkedFramework("CFNetwork"),
                .linkedFramework("CoreFoundation"),
                .linkedFramework("CoreTelephony"),
                .linkedFramework("SystemConfiguration"),
                .linkedFramework("CoreGraphics"),
                .linkedFramework("Foundation"),
                .linkedFramework("Security"),
                .linkedFramework("UserNotifications")
            ]
        ),
        .target(
            name: "MTMAWrapper",
            dependencies: ["MTPush","MTMA"], 
            path: "MTMAWrapper",
            linkerSettings: [ 
                .linkedLibrary("z"),
                .linkedLibrary("resolv"),
                .linkedFramework("UIKit"),
                .linkedFramework("CFNetwork"),
                .linkedFramework("CoreFoundation"),
                .linkedFramework("CoreTelephony"),
                .linkedFramework("SystemConfiguration"),
                .linkedFramework("CoreGraphics"),
                .linkedFramework("Foundation"),
                .linkedFramework("Security"),
                .linkedFramework("UserNotifications")
            ]
        ),
        .binaryTarget(name: "MTPush", path: "mtpush-ios-5.3.1.xcframework"),
        .binaryTarget(name: "MTMA", path: "MTMA-ios-5.3.1.xcframework")
    ]
)

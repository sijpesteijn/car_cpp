import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import {
    NgModule,
    ApplicationRef, APP_INITIALIZER
} from '@angular/core';
import {
    removeNgStyles,
    createNewHosts,
    createInputTransfer
} from '@angularclass/hmr';
import {
    RouterModule,
    PreloadAllModules
} from '@angular/router';

/*
 * Platform and Environment providers/directives/pipes
 */
import { ENV_PROVIDERS } from './environment';
import { ROUTES } from './app.routes';
// App is our top level component
import { AppComponent } from './app.component';
// import { APP_RESOLVER_PROVIDERS } from './app.resolver';
import { AppState, InternalStateType } from './app.service';
import { NoContentComponent } from './no-content';

import '../styles/styles.scss';
import '../styles/headings.css';
import { ObserverModule } from './race/observers/observer.module';
import { LifeLineComponent } from './header/lifeline/lifeline.component';
import { Config } from './app.config';
import { ObserverTabsModule } from './race/observer-tabs/observer-tabs.module';
import { StreamModule } from './webcam/stream.module';
import { LifeLineModule } from './header/lifeline/lifeline.module';
import {CarService} from "./car.service";
import {OffComponent} from "./off/off.component";
import {HeaderComponent} from "./header/header.component";
import {CarStatusComponent} from "./header/car-status/car-status.component";
import {NgxVirtualJoystickModule} from "ngx-virtual-joystick";
import {CameraStatusComponent} from "./header/camera-status/camera-status.component";
import {CameraService} from "./camera.service";
import {EventService} from "./event.service";
import {RaceService} from "./race/race.service";
import {HttpClientModule} from "@angular/common/http";
import {CommonModule} from "@angular/common";
import {RaceModule} from "./race/race.module";
import {PreviewModule} from "./preview/preview.module";
import { LOCAL_STREAM } from "./preview/preview.component";

// Application wide providers
const APP_PROVIDERS = [
    // ...APP_RESOLVER_PROVIDERS,
    AppState
];

type StoreType = {
    state: InternalStateType,
    restoreInputValues: () => void,
    disposeOldHosts: () => void
};

let PROVIDERS = [
];

if ('local' === process.env.CONTEXT) {
    PROVIDERS = [
        { provide: LOCAL_STREAM, useValue: true },
        { provide: 'settings.path', useValue: './assets/config/application.local.json' },
        { provide: APP_INITIALIZER, useFactory: (config: Config) => () => config.load('/assets/config/application.local.json'), deps: [Config], multi: true }
        ]
} else {
    PROVIDERS = [
        { provide: LOCAL_STREAM, useValue: false },
        { provide: 'settings.path', useValue: './assets/config/application.bb.json' },
        { provide: APP_INITIALIZER, useFactory: (config: Config) => () => config.load('/assets/config/application.bb.json'), deps: [Config], multi: true }
    ]
}

/**
 * `AppModule` is the main entry point into Angular2's bootstraping process
 */
@NgModule({
    bootstrap   : [AppComponent],
    declarations: [
        AppComponent,
        NoContentComponent,
        OffComponent,
        HeaderComponent,
        CarStatusComponent,
        CameraStatusComponent,
        LifeLineComponent,
    ],
    /**
     * Import Angular's modules.
     */
    imports     : [
        BrowserModule,
        CommonModule,
        FormsModule,
        HttpClientModule,
        RaceModule,
        PreviewModule,
        NgxVirtualJoystickModule.forRoot(),
        RouterModule.forRoot(ROUTES, { useHash: true, preloadingStrategy: PreloadAllModules })
    ],
    /**
     * Expose our Services and Providers into Angular's dependency injection.
     */
    providers   : [
        ENV_PROVIDERS,
        APP_PROVIDERS,
        ...PROVIDERS,
        CarService,
        CameraService,
        EventService,
        RaceService,
        Config
    ]
})
export class AppModule {

    constructor(public appRef: ApplicationRef,
                public appState: AppState) {
    }

    public hmrOnInit(store: StoreType) {
        if (!store || !store.state) {
            return;
        }
        console.log('HMR store', JSON.stringify(store, null, 2));
        /**
         * Set state
         */
        this.appState._state = store.state;
        /**
         * Set input values
         */
        if ('restoreInputValues' in store) {
            let restoreInputValues = store.restoreInputValues;
            setTimeout(restoreInputValues);
        }

        this.appRef.tick();
        delete store.state;
        delete store.restoreInputValues;
    }

    public hmrOnDestroy(store: StoreType) {
        const cmpLocation = this.appRef.components.map((cmp) => cmp.location.nativeElement);
        /**
         * Save state
         */
        const state       = this.appState._state;
        store.state       = state;
        /**
         * Recreate root elements
         */
        store.disposeOldHosts = createNewHosts(cmpLocation);
        /**
         * Save input values
         */
        store.restoreInputValues = createInputTransfer();
        /**
         * Remove styles
         */
        removeNgStyles();
    }

    public hmrAfterDestroy(store: StoreType) {
        /**
         * Display new elements
         */
        store.disposeOldHosts();
        delete store.disposeOldHosts;
    }

}

import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ObserverTabsComponent } from './observer-tabs.component';
import { ObserverModule } from '../observers/observer.module';
import { FormsModule } from '@angular/forms';
import { RouterModule } from '@angular/router';
import { observer_tabs_routes } from './observer-tabs.routes';
import {ObserverStateMessagesModule} from "../observers-state-messages/observer-state-messages.module";
import {DragRaceComponent} from "./race-types/drag-race/drag-race.component";
@NgModule({
    imports: [
        CommonModule,
        FormsModule,
        ObserverModule,
        ObserverStateMessagesModule,
        RouterModule.forChild(observer_tabs_routes)
    ],
    exports: [
        DragRaceComponent,
        ObserverTabsComponent
    ],
    declarations: [
        DragRaceComponent,
        ObserverTabsComponent,
    ]
})
export class ObserverTabsModule {

}
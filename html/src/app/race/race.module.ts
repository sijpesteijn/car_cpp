import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { RouterModule } from '@angular/router';
import {race_routes} from "./race.routes";
import {ObserverTabsComponent} from "./observer-tabs/observer-tabs.component";
import {TrafficLightObserverComponent} from "./observers/traffic-light/traffic-light.component";
import {FinishDetectionComponent} from "./observers/finish-detection/finish-detection.component";
import {LaneDetectionComponent} from "./observers/lane-detection/lane-detection.component";
import {ObserverPreviewComponent} from "./observer-preview/observer-preview.component";
import {PreviewModule} from "../preview/preview.module";
import {PreviewWithRoisComponent} from "./preview-with-rois/preview-with-rois.component";
import {RaceComponent} from "./race.component";

@NgModule({
    imports: [
        CommonModule,
        FormsModule,
        PreviewModule,
        RouterModule.forChild(race_routes)
    ],
    exports: [
        ObserverTabsComponent
    ],
    declarations: [
        FinishDetectionComponent,
        LaneDetectionComponent,
        ObserverTabsComponent,
        PreviewWithRoisComponent,
        RaceComponent,
        TrafficLightObserverComponent,
    ],
    providers: [
    ]
})
export class RaceModule {

}
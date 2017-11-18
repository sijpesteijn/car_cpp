import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { ObserverService } from './observer.service';
import { TrafficLightObserverComponent } from './traffic-light/traffic-light.component';
import { HttpModule } from '@angular/http';
import { StreamModule } from '../webcam/stream.module';
import { LaneDetectionComponent } from './lane-detection/lane-detection.component';
import {FinishDetectionComponent} from "./finish-detection/finish-detection.component";

@NgModule({
    imports: [
        CommonModule,
        FormsModule,
        HttpModule
    ],
    exports: [
        LaneDetectionComponent,
        FinishDetectionComponent,
        TrafficLightObserverComponent
    ],
    declarations: [
        LaneDetectionComponent,
        FinishDetectionComponent,
        TrafficLightObserverComponent
    ],
    providers: [
        ObserverService
    ]
})
export class ObserverModule {

}
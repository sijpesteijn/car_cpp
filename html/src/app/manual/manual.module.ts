import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ManualComponent } from './manual.component';
import { RouterModule } from '@angular/router';
import { manual_routes } from './manual.routes';
import {NgxVirtualJoystickModule} from "ngx-virtual-joystick";
import {PreviewModule} from "../preview/preview.module";

@NgModule({
    imports: [
        CommonModule,
        NgxVirtualJoystickModule,
        PreviewModule,
        RouterModule.forChild(manual_routes)
    ],
    declarations: [
        ManualComponent
    ]
})
export class ManualModule {

}
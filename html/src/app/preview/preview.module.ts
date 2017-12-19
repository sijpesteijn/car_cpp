import { NgModule } from '@angular/core';
import {PreviewComponent} from "./preview.component";
import {FormsModule} from "@angular/forms";
import {CommonModule} from "@angular/common";

@NgModule({
    imports: [
        CommonModule,
        FormsModule,
    ],
    exports: [
        PreviewComponent
    ],
    declarations: [
        PreviewComponent
    ],
    providers: [
    ]
})
export class PreviewModule {

}
import { Component } from '@angular/core';
@Component({
    selector: 'manual',
    template: require('./manual.html'),
    styles: [require('./manual.scss')]
})
export class ManualComponent {

    onChange(event: any) {
        console.log(event);
    }
}
import { Routes } from '@angular/router';
import { NoContentComponent } from './no-content';

export const ROUTES: Routes = [
    { path: 'manual', loadChildren: './manual/manual.module#ManualModule'},
    { path: 'observers', loadChildren: './observer-tabs/observer-tabs.module#ObserverTabsModule'},
    { path: 'error', loadChildren: './error/errors.module#ErrorModule' },
    { path: '**', component: NoContentComponent },
];
